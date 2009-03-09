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

IMPLEMENT_DYNAMIC_CLASS(CReadBookSimpleView, CReadBookView)

CReadBookSimpleView::CReadBookSimpleView(void) :
m_pViewPage(NULL)
{
}

CReadBookSimpleView::~CReadBookSimpleView(void)
{
    if (m_pViewPage != NULL)
        delete m_pViewPage;
}

void CReadBookSimpleView::OnDraw(wxDC *pDC)
{
	CReadBookSimpleDoc* pDoc = GetReadBookDoc();

	if (!pDoc)
		return;

	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();
	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();

    wxRect clientRect = m_pCanvas->GetClientRect();
	clientRect.Deflate(colMargin,0);

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

        if (pFileInfo != NULL)
        {
            ScrollToPosition(pFileInfo->m_nFilePos);
        }
        else
        {
            ScrollToPosition(0);
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
	wxRect rect = m_pCanvas->GetClientRect();

	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
	rect.Deflate(colMargin,0);

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
		if (nCurrentPos == ScrollToPosition(0))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_BOTTOM)
	{
		if (nCurrentPos == ScrollToLastPage())
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
		if (nCurrentPos == ScrollToPosition(event.GetPosition()))
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

    if (pFileInfo != NULL)
    {
        ScrollToPosition(pFileInfo->m_nFilePos);
    }
    else
    {
        ScrollToPosition(0);
    }

	if (m_pCanvas)
		m_pCanvas->Refresh();
    
    UpdateScrollPos();
}

wxFileOffset CReadBookSimpleView::GetCurrentPosition()
{
    if (m_pViewPage != NULL)
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
        return ScrollToPosition(0);

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

	wxRect rect = m_pCanvas->GetClientRect();

	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();

	rect.Deflate(colMargin,0);

    wxInt32 nCharsPerLine = rect.GetWidth() / m_mbFontSize.GetWidth() * 2;

    CReadBookSimpleDoc * pDoc = GetReadBookDoc();

    if (nDelta > 0)
    {
        if (m_pViewPage != NULL && m_pViewPage->GetEndFileOffset() >= m_nFileEndPosition)
            return GetCurrentPosition();

        wxFileOffset nBeginPos = GetCurrentPosition();

        if (m_pViewPage != NULL) 
            nBeginPos = m_pViewPage->GetEndFileOffset();
        else
            nDelta = m_nPageSize;

        pDoc->SeekTo(nBeginPos);

        CReadBookPage * pPage = new CReadBookPage(m_nPageSize,
            m_mbFontSize.GetHeight(),
            lineMargin,
            nCharsPerLine,
            m_mbFontSize.GetWidth() / 2,
            colMargin);

        for(wxInt32 row = 0; row < nDelta; row++)
        {
            bool end_of_file = false;
            bool end_of_line = false;

            wxChar ch = 0;

            wxFileOffset begin_offset = 0;
            wxFileOffset end_offset = 0;

            for(wxInt32 col = 0; col < nCharsPerLine; col++)
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

                    CReadBookChar * pChar = new CReadBookChar();
                    pChar->SetData(begin_offset, end_offset, ch);

                    if (!pPage->SetChar(row, col, pChar))
                    {
                        pDoc->SeekTo(begin_offset);
                        delete pChar;
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

        if (m_pViewPage == NULL)
        {
            m_pViewPage = pPage;
        }
        else
        {
            for(wxInt32 row = 0; row < nDelta; row++)
            {
                m_pViewPage->RemoveLine(row);
            }

            wxInt32 nOldLineCount = m_pViewPage->GetLineCount();

            for(wxInt32 row = nDelta; row < nOldLineCount; row++)
            {
                CReadBookLine * pLine = m_pViewPage->GetLine(row);
                m_pViewPage->RemoveLine(row, false);
                m_pViewPage->SetLine(row - nDelta, pLine);
            }

            nOldLineCount = m_pViewPage->GetLineCount();

            wxInt32 nNewLineCount = pPage->GetLineCount();

            for(wxInt32 row = 0; row < nNewLineCount; row++)
            {
                CReadBookLine * pLine = pPage->GetLine(row);
                pPage->RemoveLine(row, false);

                m_pViewPage->SetLine(nOldLineCount++, pLine);

                if (nOldLineCount >= m_nPageSize)
                    break;
            }

            delete pPage;
        }
    }
    else
    {
        wxInt32 nOffsetDelta = -nDelta * nCharsPerLine * 4;

        wxFileOffset nBeginPos = GetCurrentPosition() - nOffsetDelta;

        if (nBeginPos < 0) nBeginPos = 0;

        wxFileOffset nEndPos = GetCurrentPosition();

        pDoc->SeekTo(nBeginPos);

        CReadBookChar ** ppChars = new CReadBookChar *[nOffsetDelta];

        memset(ppChars, 0, sizeof(CReadBookChar *) * nOffsetDelta);

        wxInt32 nCount = 0;

        bool end_of_file = false;
        bool end_of_line = false;

        wxChar ch = 0;

        wxFileOffset begin_offset = 0;
        wxFileOffset end_offset = 0;

        while(true)
        {
            if (pDoc->ReadChar(ch, begin_offset, end_offset, end_of_line))
            {
                if (begin_offset >= nEndPos)
                    break;

                if (end_of_line)
                {
                    ppChars[nCount++] = NULL;
                }
                else
                {
                    CReadBookChar * pChar = new CReadBookChar();
                    pChar->SetData(begin_offset, end_offset, ch);

                    ppChars[nCount++] = pChar;
                }

                if (nCount >= nOffsetDelta)
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

            CReadBookPage * pPage = new CReadBookPage(m_nPageSize,
                m_mbFontSize.GetHeight(),
                lineMargin,
                nCharsPerLine,
                m_mbFontSize.GetWidth() / 2,
                colMargin);

            bool end = false;

            for(wxInt32 row = -nDelta - 1; row >= 0; row --)
            {
                for(wxInt32 col = nCharsPerLine - 1; col >= 0; col--)
                {
                    while (ppChars[nCount] == NULL && col == nCharsPerLine - 1)
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

                    if (ppChars[nCount] == NULL)
                        break;

                    if (!ppChars[nCount]->IsAscii())
                        col--;

                    if (col < 0)
                        break;

                    pPage->SetChar(row, col, ppChars[nCount]);
                    ppChars[nCount] = NULL;

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
            pPage->NormalizeLines();
    
            if (m_pViewPage != NULL)
            {
                wxInt32 nLineCount = pPage->GetLineCount();

                for(wxInt32 row = nLineCount; row < m_nPageSize; row++)
                {
                    CReadBookLine * pLine = m_pViewPage->GetLine(row - nLineCount);

                    m_pViewPage->RemoveLine(row - nLineCount, false);

                    pPage->SetLine(row, pLine);
                }

                delete m_pViewPage;
                m_pViewPage = pPage;
            }
            else
            {
                m_pViewPage = pPage;
            }
        }// if nCount > 0
        else if (m_pViewPage == NULL)
        {
            m_pViewPage = new CReadBookPage(m_nPageSize,
                m_mbFontSize.GetHeight(),
                lineMargin,
                nCharsPerLine,
                m_mbFontSize.GetWidth() / 2,
                colMargin);
        }

        for(wxInt32 i = 0; i < nOffsetDelta; i++)
        {
            if (ppChars[i] != NULL)
                delete ppChars[i];
        }

        delete ppChars;
    }

    wxGetApp().GetPreference()->SetFileInfo(pDoc->GetFileName(), 
        0, GetCurrentPosition());

    return GetCurrentPosition();
}

wxFileOffset CReadBookSimpleView::ScrollToPosition(wxFileOffset nPos)
{
    if (nPos == GetCurrentPosition() && m_pViewPage != NULL)
        return nPos;

    if (nPos < 0)
        nPos = 0;

	wxRect rect = m_pCanvas->GetClientRect();

	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();

	rect.Deflate(colMargin,0);

    wxInt32 nCharsPerLine = rect.GetWidth() / m_mbFontSize.GetWidth() * 2;

    CReadBookSimpleDoc * pDoc = GetReadBookDoc();

    pDoc->SeekTo(nPos);

    CReadBookPage * pPage = new CReadBookPage(m_nPageSize,
        m_mbFontSize.GetHeight(),
        lineMargin,
        nCharsPerLine,
        m_mbFontSize.GetWidth() / 2,
        colMargin);

    for(wxInt32 row = 0; row < m_nPageSize; row++)
    {
        bool end_of_file = false;
        bool end_of_line = false;

        wxChar ch = 0;

        wxFileOffset begin_offset = 0;
        wxFileOffset end_offset = 0;

        for(wxInt32 col = 0; col < nCharsPerLine; col++)
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

                CReadBookChar * pChar = new CReadBookChar();
                pChar->SetData(begin_offset, end_offset, ch);

                if (!pPage->SetChar(row, col, pChar))
                {
                    pDoc->SeekTo(begin_offset);
                    delete pChar;
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

    if (m_pViewPage != NULL)
    {
        delete m_pViewPage;
    }

    m_pViewPage = pPage;

    wxGetApp().GetPreference()->SetFileInfo(pDoc->GetFileName(), 
        0, GetCurrentPosition());

    return GetCurrentPosition();
}

void CReadBookSimpleView::OnKeyDown(wxKeyEvent& event)
{
	bool doScroll = true;

	CReadBookSimpleDoc * pDoc = GetReadBookDoc();

    wxFileOffset nCurrentPos = GetCurrentPosition();

	switch(event.GetKeyCode())
	{
	case WXK_HOME:
		if (nCurrentPos == ScrollToPosition(0))
		{
			doScroll = false;
		}
		break;
	case WXK_END:
		if (nCurrentPos == ScrollToLastPage())
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
		m_pCanvas->Refresh();
	}
}

void CReadBookSimpleView::CalculateLastCharOffset()
{
	wxRect rect = m_pCanvas->GetClientRect();

	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();

	rect.Deflate(colMargin,0);

    wxInt32 nCharsPerLine = rect.GetWidth() / m_mbFontSize.GetWidth() * 2;

    CReadBookSimpleDoc * pDoc = GetReadBookDoc();

    wxInt32 nOffsetDelta = m_nPageSize * nCharsPerLine * 4;

    wxFileOffset nBeginPos = m_nViewSize - nOffsetDelta;

    if (nBeginPos < 0) nBeginPos = 0;

    wxFileOffset nEndPos = m_nViewSize;

    pDoc->SeekTo(nBeginPos);

    CReadBookChar ** ppChars = new CReadBookChar *[nOffsetDelta];
    memset(ppChars, 0, sizeof(CReadBookChar *) * nOffsetDelta);

    wxInt32 nCount = 0;

    bool end_of_file = false;
    bool end_of_line = false;

    wxChar ch = 0;

    wxFileOffset begin_offset = 0;
    wxFileOffset end_offset = 0;

    while(true)
    {
        if (pDoc->ReadChar(ch, begin_offset, end_offset, end_of_line))
        {
            if (begin_offset >= nEndPos)
                break;

            if (end_of_line)
            {
                ppChars[nCount++] = NULL;
            }
            else
            {
                CReadBookChar * pChar = new CReadBookChar();
                pChar->SetData(begin_offset, end_offset, ch);

                ppChars[nCount++] = pChar;
            }

            if (nCount >= nOffsetDelta)
                break;
        }
        else
        {
            break;
        }
    }

    if (nCount > 0)
    {
        while(ppChars[--nCount] == NULL)
        {
            if (nCount == 0)
                break;
        }

        if (ppChars[nCount] != NULL)
        {
            m_nFileEndPosition = ppChars[nCount]->GetEndFileOffset();
        }
    }

    for(wxInt32 i = 0;i < nOffsetDelta; i++)
    {
        if (ppChars[i] != NULL)
            delete ppChars[i];
    }

    delete ppChars;
}

wxFileOffset CReadBookSimpleView::ScrollToLastPage()
{
	wxRect rect = m_pCanvas->GetClientRect();

	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();

	rect.Deflate(colMargin,0);

    wxInt32 nCharsPerLine = rect.GetWidth() / m_mbFontSize.GetWidth() * 2;

    CReadBookSimpleDoc * pDoc = GetReadBookDoc();

    wxInt32 nOffsetDelta = m_nPageSize * nCharsPerLine * 4;

    wxFileOffset nBeginPos = m_nViewSize - nOffsetDelta;

    if (nBeginPos < 0) nBeginPos = 0;

    wxFileOffset nEndPos = m_nViewSize;

    pDoc->SeekTo(nBeginPos);

    CReadBookChar ** ppChars = new CReadBookChar *[nOffsetDelta];
    memset(ppChars, 0, sizeof(CReadBookChar *) * nOffsetDelta);

    wxInt32 nCount = 0;

    bool end_of_file = false;
    bool end_of_line = false;

    wxChar ch = 0;

    wxFileOffset begin_offset = 0;
    wxFileOffset end_offset = 0;

    while(true)
    {
        if (pDoc->ReadChar(ch, begin_offset, end_offset, end_of_line))
        {
            if (begin_offset >= nEndPos)
                break;

            if (end_of_line)
            {
                ppChars[nCount++] = NULL;
            }
            else
            {
                CReadBookChar * pChar = new CReadBookChar();
                pChar->SetData(begin_offset, end_offset, ch);

                ppChars[nCount++] = pChar;
            }

            if (nCount >= nOffsetDelta)
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

        CReadBookPage * pPage = new CReadBookPage(m_nPageSize,
            m_mbFontSize.GetHeight(),
            lineMargin,
            nCharsPerLine,
            m_mbFontSize.GetWidth() / 2,
            colMargin);

        bool end = false;

        for(wxInt32 row = m_nPageSize - 1; row >= 0; row--)
        {
            for(wxInt32 col = nCharsPerLine - 1; col >= 0; col--)
            {
                while (ppChars[nCount] == NULL && col == nCharsPerLine - 1)
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

                if (ppChars[nCount] == NULL)
                    break;

                if (!ppChars[nCount]->IsAscii())
                    col--;

                if (col < 0)
                    break;

                pPage->SetChar(row, col, ppChars[nCount]);
                ppChars[nCount] = NULL;

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
        pPage->NormalizeLines();

        if (m_pViewPage != NULL)
        {
            wxInt32 nLineCount = pPage->GetLineCount();

            for(wxInt32 row = nLineCount; row < m_nPageSize; row++)
            {
                CReadBookLine * pLine = m_pViewPage->GetLine(row - nLineCount);

                m_pViewPage->RemoveLine(row - nLineCount, false);

                pPage->SetLine(row, pLine);
            }

            delete m_pViewPage;
            m_pViewPage = pPage;
        }
        else
        {
            m_pViewPage = pPage;
        }
    }
    else if (m_pViewPage == NULL)
    {
        m_pViewPage = new CReadBookPage(m_nPageSize,
            m_mbFontSize.GetHeight(),
            lineMargin,
            nCharsPerLine,
            m_mbFontSize.GetWidth() / 2,
            colMargin);
    }

    for(wxInt32 i = 0;i < nOffsetDelta; i++)
    {
        if (ppChars[i] != NULL)
            delete ppChars[i];
    }

    delete ppChars;

    wxGetApp().GetPreference()->SetFileInfo(pDoc->GetFileName(), 
        0, GetCurrentPosition());

    m_nFileEndPosition = m_pViewPage->GetEndFileOffset();

    return GetCurrentPosition();
}

void CReadBookSimpleView::OnSize(wxSizeEvent& event)
{
    Recalculate();

    if (m_pViewPage != NULL)
        delete m_pViewPage;

    m_pViewPage = NULL;
}
