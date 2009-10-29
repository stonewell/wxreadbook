#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/filesys.h"

#include "ReadBookApp.h"
#include "ReadBookMainFrm.h"
#include "ReadBookSimpleDoc.h"
#include "ReadBookSimpleView.h"
#include "ReadBookCanvas.h"
#include "ReadBookPage.h"
#include "ReadBookDC.h"
#include "ObjectCache.h"

IMPLEMENT_DYNAMIC_CLASS(CReadBookSimpleView, CReadBookView)

CReadBookSimpleView::CReadBookSimpleView(void) :
m_pViewPage(NULL),
m_nFileEndPosition(0),
m_nFileBeginPosition(0),
m_nCharsPerLine(0),
m_ppChars(NULL)
{
}

CReadBookSimpleView::~CReadBookSimpleView(void)
{
	if (m_pViewPage != NULL)
		delete m_pViewPage;

	ReleasePPChars();
}

void CReadBookSimpleView::OnDraw(wxDC *pDC)
{
	CReadBookSimpleDoc* pDoc = GetReadBookDoc();

	if (!pDoc)
		return;

	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();
	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();

	wxRect clientRect = GetClientRect();

	const wxFont & pOldFont = pDC->GetFont();

	wxFont * pNewFont = wxGetApp().GetPreference()->GetFont();
	pDC->SetFont(*pNewFont);

	const wxColour & oldColor = pDC->GetTextForeground();

	pDC->SetTextForeground(wxGetApp().GetPreference()->GetTextColor());

	const wxBrush & oldBkColor = pDC->GetBackground();

	pDC->SetBackground(wxBrush(wxGetApp().GetPreference()->GetBackgroundColor()));

	if (m_pViewPage == NULL)
	{
		CFileInfo * pFileInfo = NULL;

		wxInt32 nRow = wxGetApp().GetPreference()->GetFileInfo(pDoc->GetFileName(), &pFileInfo);

		m_pViewPage = CreateReadBookPage();

		if (pFileInfo != NULL)
		{
			ScrollToBeginPosition(pFileInfo->m_nFilePos);
		}
		else
		{
			ScrollToBeginPosition(0);
		}
	}

	m_pViewPage->Paint(clientRect.GetLeft(), clientRect.GetTop(), pDC);

	pDC->SetFont(pOldFont);
	pDC->SetTextForeground(oldColor);
	pDC->SetBackground(oldBkColor);
}

void CReadBookSimpleView::CalculateViewSize()
{
	CReadBookSimpleDoc * pDoc = GetReadBookDoc();

	m_nViewSize = pDoc->GetBufferSize();
}

void CReadBookSimpleView::CalculateScrollSize(void)
{
	wxRect rect  = GetClientRect();

	m_nPageSize = rect.GetHeight() /
		(m_FontSize.GetHeight() + wxGetApp().GetPreference()->GetLineMargin());

	if (m_nPageSize <= 0)
		m_nPageSize = 1;

	if (m_nViewSize < m_nPageSize)
		m_nPageSize = m_nViewSize;

	CReadBookSimpleDoc * pBufferedDoc = 
		(CReadBookSimpleDoc *)GetReadBookDoc();

	wxFileOffset pos = GetCurrentPosition();

	m_pCanvas->SetScrollbar(wxVERTICAL,
		pos,
		m_nPageSize,
		m_nViewSize);
}

void CReadBookSimpleView::UpdateScrollPos(void)
{
	m_pCanvas->SetScrollPos(wxVERTICAL, 
		GetCurrentPosition());
}

void CReadBookSimpleView::OnScrollWin(wxScrollWinEvent& event)
{
	bool doScroll = true;

	wxFileOffset nCurrentPos = GetCurrentPosition();

	if (event.GetEventType() == wxEVT_SCROLLWIN_TOP)
	{
		if (nCurrentPos == ScrollToBeginPosition(m_nFileBeginPosition))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_BOTTOM)
	{
		if (nCurrentPos == ScrollToEndPosition(m_nFileEndPosition))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEUP)
	{
		if ( nCurrentPos == ScrollSimpleLine(-1))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN)
	{
		if ( nCurrentPos == ScrollSimpleLine(1))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEUP)
	{
		if (nCurrentPos == ScrollSimplePage(-1))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEDOWN)
	{
		if (nCurrentPos == ScrollSimplePage(1))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK)
	{
		if (nCurrentPos == ScrollToBeginPosition(event.GetPosition()))
		{
			doScroll = false;
		}
	}
	else
	{
		doScroll = false;
	}

	if (doScroll)
	{
		UpdateScrollPos();

		m_pCanvas->Refresh();
	}
}

void CReadBookSimpleView::OnMouseWheel(wxMouseEvent & event)
{
	wxInt32 nDeltaLine = event.GetWheelDelta() /
		(m_FontSize.GetHeight() + wxGetApp().GetPreference()->GetLineMargin());

	if (nDeltaLine <= 0)
		nDeltaLine = 1;

	bool doScroll = true;

	wxFileOffset nCurrentPos = GetCurrentPosition();

	if (event.GetWheelRotation() < 0)
	{
		if (nDeltaLine < 0)
			nDeltaLine *= -1;
	}
	else
	{
		if (nDeltaLine > 0)
			nDeltaLine *= -1;
	}

	if (event.IsPageScroll())
	{
		if (nCurrentPos == ScrollSimplePage(nDeltaLine))
			doScroll = false;
	}
	else
	{
		if (nCurrentPos == ScrollSimplePage(nDeltaLine))
			doScroll = false;
	}

	if (doScroll)
	{
		UpdateScrollPos();

		m_pCanvas->Refresh();
	}
}

void CReadBookSimpleView::OnUpdate(wxView *sender, wxObject *hint)
{
	wxString url = GetReadBookDoc()->GetFileName();

	if (url.Upper().EndsWith(wxT(".HTML")) || url.Upper().EndsWith(wxT(".HTM")))
	{
		m_ViewMode = wxReadBook::Html;
	}
	else
	{
		m_ViewMode = wxReadBook::Text;
	}

	CFileInfo * pFileInfo = NULL;

	wxInt32 nRow = wxGetApp().GetPreference()->GetFileInfo(url, &pFileInfo);

	Recalculate();

	CalculateLastCharOffset();
	CalculateFirstCharOffset();

	if (pFileInfo != NULL)
	{
		ScrollToBeginPosition(pFileInfo->m_nFilePos);
	}
	else
	{
		ScrollToBeginPosition(m_nFileBeginPosition);
	}

	if (m_pCanvas)
		m_pCanvas->Refresh();

	UpdateScrollPos();
}

void CReadBookSimpleView::OnKeyDown(wxKeyEvent& event)
{
	bool doScroll = true;

	CReadBookSimpleDoc * pDoc = GetReadBookDoc();

	wxFileOffset nCurrentPos = GetCurrentPosition();

	switch(event.GetKeyCode())
	{
	case WXK_HOME:
		if (nCurrentPos == ScrollToBeginPosition(m_nFileBeginPosition))
		{
			doScroll = false;
		}
		break;
	case WXK_END:
		if (nCurrentPos == ScrollToEndPosition(m_nFileEndPosition))
		{
			doScroll = false;
		}
		break;
	case 'A':
	case 'a':
	case WXK_PAGEUP:
		if ( nCurrentPos == ScrollSimplePage(-1))
		{
			doScroll = false;
		}
		break;
	case 'D':
	case 'd':
	case WXK_SPACE:
	case WXK_PAGEDOWN:
		if ( nCurrentPos == ScrollSimplePage(1))
		{
			doScroll = false;
		}
		break;
	case WXK_UP:
		if ( nCurrentPos == ScrollSimpleLine(-1))
		{
			doScroll = false;
		}
		break;
	case WXK_DOWN:
		if ( nCurrentPos == ScrollSimpleLine(1))
		{
			doScroll = false;
		}
		break;
	case 'h':
	case 'H':
		if (GetCurrentPosition() > 0)
		{
			pDoc->SeekTo(GetCurrentPosition() - 1);
		}
		break;
	case 'F':
	case 'f':
		{
			CReadBookMainFrm * pMainFrame = 
				(CReadBookMainFrm *)(GetMainFrame());
			wxCommandEvent event;
			pMainFrame->OnFullScreen(event);
		}
		break;
	default:
		CReadBookView::OnKeyDown(event);
		return;
	}

	if (doScroll)
	{
		UpdateScrollPos();
		m_pCanvas->Refresh();
	}
}


wxFileOffset CReadBookSimpleView::GetCurrentPosition()
{
	if (m_pViewPage != NULL && m_pViewPage->GetLineCount() > 0)
		return m_pViewPage->GetBeginFileOffset();

	wxString url = GetReadBookDoc()->GetFileName();

	CFileInfo * pFileInfo = NULL;

	wxInt32 nRow = wxGetApp().GetPreference()->GetFileInfo(url, &pFileInfo);

	if (pFileInfo != NULL)
		return pFileInfo->m_nFilePos;

	return 0;
}

wxFileOffset CReadBookSimpleView::ScrollSimplePage(wxInt16 nDelta)
{
	wxInt32 nCount = nDelta > 0 ? nDelta : -nDelta;

	wxInt32 nLineCount = m_pViewPage == NULL ? 0 : m_pViewPage->GetLineCount();

	wxFileOffset offset = 0;

	if (nLineCount == 0)
		return ScrollToBeginPosition(m_nFileBeginPosition);

	for(wxInt32 i = 0; i < nCount; i++)
	{
		if (nDelta > 0)
		{
			ScrollSimpleLine(m_nPageSize - 1);
		}
		else
		{
			ScrollSimpleLine(-1 * (m_nPageSize - 1));
		}

		nLineCount = m_pViewPage->GetLineCount();

		if (nLineCount < m_nPageSize)
			break;
	}

	return GetCurrentPosition();
}

wxFileOffset CReadBookSimpleView::ScrollSimpleLine(wxInt16 nDelta)
{
	if (nDelta == 0)
		return GetCurrentPosition();

	if (nDelta > 0)
	{
		if (m_pViewPage != NULL && 
			m_pViewPage->GetEndFileOffset() >= m_nFileEndPosition)
			return GetCurrentPosition();

		if (m_pViewPage == NULL)
			return ScrollToBeginPosition(GetCurrentPosition());

		while (nDelta > 0)
		{
			wxInt32 nLineCount = m_pViewPage->GetLineCount();

			if (nDelta >= nLineCount)
			{
				ScrollToBeginPosition(m_pViewPage->GetEndFileOffset());
				nDelta -= nLineCount;
			}
			else
			{
				CReadBookLine * pLine = m_pViewPage->GetLine(nDelta);

				ScrollToBeginPosition(pLine->GetBeginFileOffset());

				nDelta = 0;
			}

			if (m_pViewPage->GetEndFileOffset() >= m_nFileEndPosition)
				break;//end of file
		}
	}
	else
	{
		if (m_pViewPage != NULL && 
			m_pViewPage->GetBeginFileOffset() <= m_nFileBeginPosition)
			return GetCurrentPosition();

		if (m_pViewPage == NULL)
			return ScrollToEndPosition(GetCurrentPosition());

		while (nDelta < 0)
		{
			wxInt32 nLineCount = m_pViewPage->GetLineCount();

			if (-nDelta >= nLineCount)
			{
				ScrollToEndPosition(m_pViewPage->GetBeginFileOffset());
				nDelta += nLineCount;
			}
			else
			{
				CReadBookLine * pLine = m_pViewPage->GetLine(nLineCount + nDelta);

				ScrollToEndPosition(pLine->GetBeginFileOffset());

				nDelta = 0;
			}

			if (m_pViewPage->GetBeginFileOffset() <= m_nFileBeginPosition)
				break;//end of file
		}
	}

	return GetCurrentPosition();
}

void CReadBookSimpleView::CalculateLastCharOffset()
{
	ScrollToEndPosition(m_nViewSize, false);
	m_nFileEndPosition = m_pViewPage->GetEndFileOffset();
}

void CReadBookSimpleView::CalculateFirstCharOffset()
{
	ScrollToBeginPosition(0, false);

	m_nFileBeginPosition = m_pViewPage->GetBeginFileOffset();
}

void CReadBookSimpleView::OnSize(wxSizeEvent& event)
{
	wxFileOffset current_pos = GetCurrentPosition();

	Recalculate();

	CReadBookSimpleDoc * pDoc = GetReadBookDoc();

	if (pDoc->GetFileName().Length() > 0)
	{
		ScrollToBeginPosition(current_pos);
	}
}

void CReadBookSimpleView::PreferenceChanged()
{
	wxFileOffset current_pos = GetCurrentPosition();

	Recalculate();

	ScrollToBeginPosition(current_pos);
}

wxRect CReadBookSimpleView::GetClientRect()
{
	wxRect rect = m_pCanvas->GetClientRect();

	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();

	rect.Deflate(colMargin,0);

	rect.SetWidth(rect.GetWidth());

	return rect;
}

wxFileOffset CReadBookSimpleView::ScrollToBeginPosition(wxFileOffset nPos, bool bUpdateFileInfo)
{
	if (nPos == GetCurrentPosition() && 
		m_pViewPage != NULL && 
		m_pViewPage->GetLineCount() > 0)
		return nPos;

	if (nPos < 0)
		nPos = 0;

	CReadBookDC dc(m_pCanvas);

	CReadBookPage * pPage = m_pViewPage;
	pPage->Clear();

	CReadBookSimpleDoc * pDoc = GetReadBookDoc();

	pDoc->SeekTo(nPos);

	wxInt32 nBeginRow = 0;

	bool end_of_file = false;
	bool end_of_line = false;
	int initial_tag_status = 0;
	int initial_script_status = 0;

	do
	{
		end_of_file = false;
		end_of_line = false;

		for(wxInt32 row = nBeginRow; row < m_nPageSize; row++)
		{
			wxChar ch = 0;

			wxFileOffset begin_offset = 0;
			wxFileOffset end_offset = 0;

			for(wxInt32 col = 0; col < m_nCharsPerLine; col++)
			{
				if (pDoc->ReadChar(ch, begin_offset, end_offset, end_of_line))
				{
					while (end_of_line && col == 0)
					{
						//first char maybe the EOL of last line
						if (!pDoc->ReadChar(ch, begin_offset, end_offset, end_of_line))
						{
							end_of_file = true;
							break;
						}
					}

					if (end_of_file)
						break;

					if (end_of_line)
						break;

					CReadBookChar * pChar = CreateReadBookChar(this);
					pChar->SetData(begin_offset, end_offset, ch);

					if (!pPage->SetChar(row, col, pChar,&dc))
					{
						pDoc->SeekTo(begin_offset);
						ReleaseReadBookChar(pChar);
						break;
					}

					if (!pChar->IsAscii())
						col++;
				}
				else
				{
					end_of_file = true;
					break;
				}
			}

			if (end_of_file)
				break;
		}//for row

		pPage->TrimEmptyLines();

		if (GetViewMode() == wxReadBook::Html)
		{
			pPage->StripHtml(&dc, nBeginRow, m_nPageSize - 1, 
				initial_tag_status, initial_script_status);
		}

		nBeginRow = pPage->GetLineCount();
	}
	while(!end_of_file && pPage->GetLineCount() < m_nPageSize);

	pPage->NormalizeLines();

	if (bUpdateFileInfo)
	{
		wxGetApp().GetPreference()->SetFileInfo(pDoc->GetFileName(), 
			0, GetCurrentPosition());
	}

	return GetCurrentPosition();
}

wxFileOffset CReadBookSimpleView::ScrollToEndPosition(wxFileOffset nPos, 
													  bool bUpdateFileInfo)
{
	if (m_pViewPage != NULL && 
		nPos == m_pViewPage->GetEndFileOffset() &&
		m_pViewPage->GetLineCount() > 0)
		return GetCurrentPosition();

	CReadBookDC dc(m_pCanvas);

	CReadBookSimpleDoc * pDoc = GetReadBookDoc();

	if (nPos > m_nViewSize)
		nPos = m_nViewSize;

	wxFileOffset nEndPos = nPos;
	wxFileOffset nBeginPos = nEndPos - m_nPageChars;

	if (nBeginPos < 0) 
		nBeginPos = 0;

	CReadBookPage * pPage = m_pViewPage;
	pPage->Clear();

	wxInt32 nBeginRow = m_nPageSize - 1;

	wxInt32 nCount = 0;

	wxInt32 tmpLineCount = 0;

	int initial_tag_status = 0;
	int initial_script_status = 0;

	do
	{
		bool end_of_line = false;
		wxChar ch = 0;

		wxFileOffset begin_offset = 0;
		wxFileOffset end_offset = 0;

		pDoc->SeekTo(nBeginPos);

		nCount = 0;

		while(true)
		{
			if (pDoc->ReadChar(ch, begin_offset, end_offset, end_of_line))
			{
				if (begin_offset >= nEndPos)
					break;

				nCount++;

				if (end_of_line)
				{
					if (m_ppChars[nCount] != NULL)
					{
						ReleaseReadBookChar(m_ppChars[nCount]);
					}

					m_ppChars[nCount] = NULL;
				}
				else
				{
					CReadBookChar * pChar = m_ppChars[nCount];

					if (pChar == NULL)
						pChar = CreateReadBookChar(this);

					pChar->SetData(begin_offset, end_offset, ch);

					m_ppChars[nCount] = pChar;
				}

				if (nCount >= m_nPageChars)
					break;
			}
			else
			{
				break;
			}
		}

		if (nCount > 0)
		{
			nCount--;

			bool end = false;

			for(wxInt32 row = nBeginRow; row >= 0; row --)
			{
				for(wxInt32 col = m_nCharsPerLine - 1; col >= 0; col--)
				{
					while (m_ppChars[nCount] == NULL && col == m_nCharsPerLine - 1)
					{
						nCount--;

						if (nCount < 0)
						{
							end = true;
							break;
						}
					}

					if (end)
						break;

					if (m_ppChars[nCount] == NULL)
						break;

					if (!m_ppChars[nCount]->IsAscii())
						col--;

					if (col < 0)
						break;

					pPage->SetChar(row, col, m_ppChars[nCount], &dc);
					m_ppChars[nCount] = NULL;//no need release, since already save to page

					nCount--;

					if (nCount < 0)
					{
						end = true;
						break;
					}
				}

				if (end)
					break;
			}// for row

			pPage->TrimEmptyLines();

			tmpLineCount = pPage->GetLineCount();

			if (tmpLineCount > 0)
				nEndPos = pPage->GetBeginFileOffset();

			if (GetViewMode() == wxReadBook::Html)
			{
				pPage->StripHtml(&dc, 0, nBeginRow, 
					initial_tag_status, initial_script_status);

				tmpLineCount = pPage->GetLineCount();
			}

			pPage->TrimEmptyLines(false);

			nBeginRow = m_nPageSize - tmpLineCount - 1;
			nBeginPos = nEndPos - m_nPageChars;

			if (nBeginPos < 0)  
				nBeginPos = 0;
		}// if nCount > 0
	}	
	while(nCount > 0 && tmpLineCount < m_nPageSize);

	pPage->TrimEmptyLines();
	pPage->NormalizeLines();

	if (pPage->GetBeginFileOffset() == m_nFileBeginPosition &&
		pPage->GetEndFileOffset() != m_nFileEndPosition &&
		pPage->GetLineCount() < m_nPageSize)
	{
		m_pViewPage->Clear();
		return ScrollToBeginPosition(m_nFileBeginPosition);
	}

	if (bUpdateFileInfo)
	{
		wxGetApp().GetPreference()->SetFileInfo(pDoc->GetFileName(), 
			0, GetCurrentPosition());
	}

	return GetCurrentPosition();
}

CReadBookPage * CReadBookSimpleView::CreateReadBookPage()
{
	wxRect rect  = GetClientRect();

	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();

	CReadBookPage * pPage = new CReadBookPage(this, 
		m_nPageSize,
		rect.GetWidth(),
		m_mbFontSize.GetHeight(),
		lineMargin,
		m_nCharsPerLine,
		m_mbFontSize.GetWidth() / 2,
		colMargin);

	return pPage;
}

void CReadBookSimpleView::Recalculate()
{
	CReadBookView::Recalculate();

	wxRect rect  = GetClientRect();

	m_nCharsPerLine = rect.GetWidth() / m_mbFontSize.GetWidth() * 2;

	ReleasePPChars();
	CreatePPChars();

	if (m_pViewPage != NULL) delete m_pViewPage;

	m_pViewPage = CreateReadBookPage();
}

void CReadBookSimpleView::ReleasePPChars()
{
	if (m_ppChars != NULL)
	{
		for(wxInt32 i = 0; i < m_nPageChars; i++)
		{
			if (m_ppChars[i] != NULL)
				ReleaseReadBookChar(m_ppChars[i]);
		}

		delete m_ppChars;
	}

	m_ppChars = NULL;
}

void CReadBookSimpleView::CreatePPChars()
{
	m_nPageChars = m_nPageSize * m_nCharsPerLine;

	m_ppChars = new CReadBookChar *[m_nPageChars];
	memset(m_ppChars, 0, sizeof(CReadBookChar *) * m_nPageChars);
}
