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

#include "wx/tokenzr.h"
#include "wx/filename.h"

#include "ReadBookApp.h"
#include "ReadBookMainFrm.h"
#include "ReadBookDoc.h"
#include "ReadBookView.h"
#include "ReadBookCanvas.h"

#if wxUSE_UNICODE
#include "nsDetector.h"
#include "GBBig5Table.h"
#endif

const CReadBookView::LineNumberMapping ZeroMap = {0,0};

extern const wxString & STANDARD_LINE =
wxT("ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890,?!.()+-");

#if wxUSE_UNICODE
extern const wxString & STANDARD_LINE_MB =
wxT("你好挨打啊挖出赫赫有名的确他山之石科研项目你么啊大案要案屗骂拉帮结伙");
#endif

IMPLEMENT_DYNAMIC_CLASS(CReadBookView, wxView)

BEGIN_EVENT_TABLE(CReadBookView, wxView)
END_EVENT_TABLE()

CReadBookView::CReadBookView() :
m_pFrame(NULL),
m_pCanvas(NULL),
m_ViewMode(wxReadBook::Text),
m_bInScript(false),
m_ClientSize(0,0),
m_DisplayAs(wxReadBook::DisplayAsOriginal)
{
}

CReadBookView::~CReadBookView()
{
}

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool CReadBookView::OnCreate(wxDocument * WXUNUSED(doc), long WXUNUSED(flags) )
{
	m_pFrame = GetMainFrame();

	m_pCanvas = ((CReadBookMainFrm *)GetMainFrame())->GetCanvas();

	m_pCanvas->SetView(this);

	// Associate the appropriate frame with this view.
	SetFrame(m_pFrame);

	// Make sure the document manager knows that this is the
	// current view.
	Activate(true);

	Recalculate();

	return true;
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void CReadBookView::OnDraw(wxDC *pDC)
{
	CReadBookDoc* pDoc = (CReadBookDoc *)GetReadBookDoc();
	if (!pDoc)
		return;

	wxRect clientRect = m_pCanvas->GetClientRect();

	const wxFont & pOldFont = pDC->GetFont();

	wxFont * pNewFont = wxGetApp().GetPreference()->GetFont();
	pDC->SetFont(*pNewFont);

	const wxColour & oldColor = pDC->GetTextForeground();

	pDC->SetTextForeground(wxGetApp().GetPreference()->GetTextColor());

	const wxBrush & oldBkColor = pDC->GetBackground();

	pDC->SetBackground(wxBrush(wxGetApp().GetPreference()->GetBackgroundColor()));

	wxInt16 y = 0;

	wxInt32 currentLine = pDoc->GetCurrentLine();

	LineNumberMapping map = ViewLineCountToDocLine(currentLine);

	wxInt32 row = map.nDocLineCount;

	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();
	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();

	wxInt32 lineCount = map.nViewLineCountBegin;

	clientRect.Deflate(colMargin,0);

	wxInt32 nStartRow = -1;

	while(row < pDoc->GetBufferSize())
	{
		wxArrayString * pLines = GetLineStrings(row);

		LineNumberMapping mapping;

		mapping.nDocLineCount = row;
		mapping.nViewLineCountBegin = lineCount;
		m_LineNumberMap[mapping.nDocLineCount] = mapping;

		for(size_t i=0;i < pLines->Count();i++)
		{
			const wxString & line = pLines->Item(i);

			wxInt64 length = line.Length();

			const wxChar * pBuf = line.GetData();

			wxSize size = pDC->GetTextExtent(line);

			if (size.GetWidth() <= clientRect.GetWidth())
			{
				wxRect rect(clientRect.GetLeft(),
					y,clientRect.GetWidth(),size.GetHeight());

				if (lineCount >= pDoc->GetCurrentLine())
				{
					if (nStartRow < 0)
						nStartRow = row;

					pDC->DrawText(line, rect.GetLeft(), rect.GetTop());

					y += size.GetHeight();

					//Line Margin
					y+=lineMargin;
				}

				lineCount++;
			}
			else
			{
				wxInt16 lineCharSize = length * clientRect.GetWidth() / size.GetWidth() + 1;
				wxInt64 startIndex = 0;

				while(startIndex < length)
				{
					lineCharSize = GetViewLineCharSize(pDC,
						pBuf,length,startIndex,lineCharSize);

					if (lineCount >= pDoc->GetCurrentLine())
					{
						wxRect rect(clientRect.GetLeft(),y,
							clientRect.GetWidth(),size.GetHeight());

						if (nStartRow < 0)
							nStartRow = row;

						pDC->DrawText(wxString(&pBuf[startIndex],
							lineCharSize),
							rect.GetLeft(), rect.GetTop());

						y += size.GetHeight();
						y += lineMargin;

						if (y >= clientRect.GetHeight())
							break;
					}

					startIndex += lineCharSize;
					lineCount++;
				}
			}

			if (y >= clientRect.GetHeight())
				break;
		}//for each line

		delete pLines;

		if (y >= clientRect.GetHeight())
			break;

		row++;
	}//while

	pDC->SetFont(pOldFont);
	pDC->SetTextForeground(oldColor);
	pDC->SetBackground(oldBkColor);
}

void CReadBookView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
	wxFileName fn(GetReadBookDoc()->GetFileName());

	if (fn.GetExt().CmpNoCase(wxT("html")) == 0 || fn.GetExt().CmpNoCase(wxT("htm")) == 0)
	{
		m_ViewMode = wxReadBook::Html;
	}
	else
	{
		m_ViewMode = wxReadBook::Text;
	}

	m_bInScript = false;

	m_LineNumberMap.clear();

	Recalculate();

	if (m_pCanvas)
		m_pCanvas->Refresh();
}

// Clean up windows used for displaying the view.
bool CReadBookView::OnClose(bool WXUNUSED(deleteWindow))
{
	if (!GetReadBookDoc()->Close())
		return false;

	m_pCanvas->ClearBackground();
	m_pCanvas->SetView(NULL);
	m_pCanvas = NULL;

	wxString s(wxTheApp->GetAppName());

	if (m_pFrame)
		m_pFrame->SetTitle(s);

	SetFrame((wxFrame *) NULL);

	Activate(false);

	m_LineNumberMap.clear();

	return true;
}

void CReadBookView::Recalculate(void)
{
	bool bFontChange = CalculateFontSize();

	wxSize tmpSize = m_pCanvas->GetClientSize();

	if (tmpSize.GetWidth() != m_ClientSize.GetWidth() || bFontChange || m_LineNumberMap.size() == 0)
	{
		CalculateViewSize();
	}

	CalculateScrollSize();

	m_ClientSize = m_pCanvas->GetClientSize();
}

bool CReadBookView::CheckCharConsistence(const wxChar * pBuf, wxInt16 startIndex, wxInt16 count) const
{
#if !wxUSE_UNICODE
	for(wxInt16 i=0;i<count;i++)
	{
		if ((pBuf[startIndex + i] & 0x80) == 0x80)
		{
			if (i + 1 < count)
			{
				i++;
			}
			else
			{
				return false;
			}
		}
	}
#endif

	return true;
}

void CReadBookView::CalculateScrollSize(void)
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

	m_pCanvas->SetScrollbar(wxVERTICAL,
		GetReadBookDoc()->GetCurrentLine(),
		m_nPageSize,
		m_nViewSize);
}

wxInt32 CReadBookView::ScrollToLine(wxInt32 nPos)
{
	wxInt32 nCurrentLine = NormalizeScrollToLine(nPos);
	GetReadBookDoc()->SetCurrentLine(nCurrentLine);
	return nCurrentLine;
}

wxInt32 CReadBookView::ScrollLine(wxInt16 nDelta)
{
	wxInt32 nCurrentLine = GetReadBookDoc()->GetCurrentLine();

	if (nCurrentLine + nDelta >= 0)
	{
		nCurrentLine += nDelta;
	}
	else
	{
		nCurrentLine = 0;
	}

	nCurrentLine = NormalizeScrollToLine(nCurrentLine);

	GetReadBookDoc()->SetCurrentLine(nCurrentLine);

	return nCurrentLine;
}

wxInt32 CReadBookView::NormalizeScrollToLine(wxInt32 nCurrentLine)
{
	LineNumberMapping map = ViewLineCountToDocLine(nCurrentLine);

	if (map.nDocLineCount == (GetReadBookDoc()->GetBufferSize() - 1) &&
		nCurrentLine > map.nViewLineCountBegin)
	{
		if (m_nLastLineViewSize <= m_nPageSize)
		{
			return map.nViewLineCountBegin;
		}
		
		if (nCurrentLine - map.nViewLineCountBegin < m_nLastLineViewSize - m_nPageSize)
		{
			return nCurrentLine;
		}

		return map.nViewLineCountBegin + m_nLastLineViewSize - m_nPageSize;
	}

	return nCurrentLine;
}

wxInt16 CReadBookView::GetPageSize(void)
{
	return m_nPageSize;
}

wxInt16 CReadBookView::SetPageSize(wxInt16 nPageSize)
{
	return m_nPageSize = nPageSize;
}

wxInt32 CReadBookView::ScrollPage(wxInt16 nDelta)
{
	return ScrollLine(nDelta * (m_nPageSize > 1 ? (m_nPageSize - 1) : m_nPageSize));
}

wxArrayString * CReadBookView::GetLineStrings(wxInt32 row)
{
	const wxString & line = GetReadBookDoc()->GetLine(row);

	if (m_ViewMode == wxReadBook::Text) {
		wxArrayString * lines = new wxArrayString();

		//if (!GetReadBookDoc()->IsEmptyLine(line))
		lines->Add(TransformHtml(line));

		return lines;
	}

	return StripHtml(line);
}

const CReadBookView::LineNumberMapping & CReadBookView::ViewLineCountToDocLine(wxInt32 viewLine)
{
	if (viewLine <= 0)
		return ZeroMap;

	wxInt32 maxKey = -1;
	wxInt32 maxKey2 = -1;

	CLineNumberMap::iterator it;

	for(it = m_LineNumberMap.begin();it != m_LineNumberMap.end();++it)
	{
		wxInt32 key = it->first;
		LineNumberMapping map = it->second;

		if (viewLine >= map.nViewLineCountBegin && key > maxKey)
		{
			maxKey = key;
		}

		if (key > maxKey2)
		{
			maxKey2 = key;
		}
	}

	if (maxKey >= 0)
	{
		return m_LineNumberMap[maxKey];
	}

	if (maxKey2 >= 0)
	{
		return m_LineNumberMap[maxKey2];
	}

	return ZeroMap;
}

wxInt16 CReadBookView::GetViewLineCharSize(wxDC * pDC,
										   const wxChar * pBuf, size_t length,
										   size_t startIndex, wxInt16 defaultLineCharSize) const
{
	wxRect clientRect = m_pCanvas->GetClientRect();

	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
	clientRect.Deflate(colMargin,0);

	wxInt16 lineCharSize = defaultLineCharSize;

	if (startIndex + lineCharSize >= length)
	{
		lineCharSize = length - startIndex;
	}

	wxSize size2 = pDC->GetTextExtent(wxString(&pBuf[startIndex],lineCharSize));

	while(size2.GetWidth() < clientRect.GetWidth() && (lineCharSize + startIndex < length))
	{
		lineCharSize++;
		size2 = pDC->GetTextExtent(wxString(&pBuf[startIndex],lineCharSize));
	}

	if (size2.GetWidth() > clientRect.GetWidth())
	{
		lineCharSize--;
	}

	if (!CheckCharConsistence(pBuf,startIndex,lineCharSize))
	{
		lineCharSize--;
	}

	return lineCharSize;
}

void CReadBookView::CalculateViewSize()
{
	//Clear line info
	m_LineNumberMap.clear();

	m_nViewSize = 0;

	if (GetReadBookDoc()->GetBufferSize() > 0)
	{
		wxRect clientRect = m_pCanvas->GetClientRect();

		wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
		clientRect.Deflate(colMargin,0);

		m_nLastLineViewSize = 1;
		wxInt32 row = 0;

		while(row < GetReadBookDoc()->GetBufferSize())
		{
			LineNumberMapping mapping;

			mapping.nDocLineCount = row;
			mapping.nViewLineCountBegin = m_nViewSize;
			m_LineNumberMap[mapping.nDocLineCount] = mapping;

			wxArrayString * pLines = GetLineStrings(row);

			for(size_t i=0;i < pLines->Count();i++)
			{
				const wxString & line = pLines->Item(i);

#if wxUSE_UNICODE
				size_t totalLength = 0;
				wxInt32 delta = 0;

				for(size_t i=0; i<line.Length(); i++) 
				{
					if ((0xFF80 & line[i]) == 0) 
					{
						totalLength += m_FontSize.GetWidth();
					}
					else
					{
						totalLength += m_mbFontSize.GetWidth();
					}

					if (totalLength > (size_t)clientRect.GetWidth())
					{
						delta++;

						if ((0xFF80 & line[i]) == 0) 
						{
							totalLength = m_FontSize.GetWidth();
						}
						else
						{
							totalLength = m_mbFontSize.GetWidth();
						}
					}
				}

				if (totalLength > 0)
					delta++;

				m_nViewSize += delta;
#else
				wxInt32 delta = line.Length() * m_FontSize.GetWidth() / clientRect.GetWidth() + 1;

				m_nViewSize += delta;
#endif
				if (row == GetReadBookDoc()->GetBufferSize() - 1)
				{
					m_nLastLineViewSize = delta;
				}
			}

			delete pLines;

			row++;
		}
	}
}

void CReadBookView::UpdateScrollPos(void)
{
	m_pCanvas->SetScrollPos(wxVERTICAL, GetReadBookDoc()->GetCurrentLine());
}

void CReadBookView::OnScrollWin(wxScrollWinEvent& event)
{
	bool doScroll = true;

	wxInt32 nCurrentLine = GetReadBookDoc()->GetCurrentLine();

	if (event.GetEventType() == wxEVT_SCROLLWIN_TOP)
	{
		if (nCurrentLine == ScrollToLine(0))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_BOTTOM)
	{
		if (nCurrentLine == ScrollToLine(ScrollPosToLine(m_nViewSize)))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEUP)
	{
		if ( nCurrentLine == ScrollLine(-1))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN)
	{
		if ( nCurrentLine == ScrollLine(1))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEUP)
	{
		if (nCurrentLine == ScrollPage(-1))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEDOWN)
	{
		if (nCurrentLine == ScrollPage(1))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK)
	{
		if (nCurrentLine == ScrollToLine(ScrollPosToLine(event.GetPosition())))
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

void CReadBookView::OnKeyDown(wxKeyEvent& event)
{
	bool doScroll = true;

	wxInt32 nCurrentLine = GetReadBookDoc()->GetCurrentLine();

	switch(event.GetKeyCode())
	{
	case WXK_HOME:
		if (nCurrentLine == ScrollToLine(0))
		{
			doScroll = false;
		}
		break;
	case WXK_END:
		if (nCurrentLine == ScrollToLine(ScrollPosToLine(m_nViewSize)))
		{
			doScroll = false;
		}
		break;
	case 'A':
	case 'a':
	case WXK_PAGEUP:
		if ( nCurrentLine == ScrollPage(-1))
		{
			doScroll = false;
		}
		break;
	case 'D':
	case 'd':
	case WXK_SPACE:
	case WXK_PAGEDOWN:
		if ( nCurrentLine == ScrollPage(1))
		{
			doScroll = false;
		}
		break;
	case WXK_UP:
		if ( nCurrentLine == ScrollLine(-1))
		{
			doScroll = false;
		}
		break;
	case WXK_DOWN:
		if ( nCurrentLine == ScrollLine(1))
		{
			doScroll = false;
		}
		break;
	case 'n':
	case 'N':
	case WXK_RETURN:
	case WXK_RIGHT:
		{
			wxGetApp().OpenRelatedFile(GetReadBookDoc()->GetFileName(), 1);
			doScroll = false;
		}
		break;
	case 'p':
	case 'P':
	case WXK_BACK:
	case WXK_LEFT:
		{
			wxGetApp().OpenRelatedFile(GetReadBookDoc()->GetFileName(),-1);
			doScroll = false;
		}
		break;
#ifdef __WXMSW__
	case WXK_ESCAPE:
		{
			wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, IDM_HIDE);

			GetMainFrame()->GetEventHandler()->ProcessEvent(event);
			break;
		}
#endif
	default:
		doScroll = false;
		event.Skip();
		break;
	}

	if (doScroll)
	{
		UpdateScrollPos();

		m_pCanvas->Refresh();
	}
}

void CReadBookView::OnSize(wxSizeEvent& WXUNUSED(event))
{
	Recalculate();
}

void CReadBookView::SetViewMode(wxReadBook::ViewModeEnum ViewMode)
{
	m_ViewMode = ViewMode;
	m_bInScript = false;

	Recalculate();

	CReadBookDoc* pDoc = GetReadBookDoc();

	pDoc->UpdateDisplay(m_DisplayAs, m_ViewMode);

	m_pCanvas->Refresh();
}

wxArrayString * CReadBookView::StripHtml(const wxString & line)
{
	const wxString delimiter(wxT("<>"));

	bool bInHtmlTag = false;
	wxString tag = wxT("");
	wxString tmpLine = wxT("");

	wxArrayString * pLines = new wxArrayString();

	for(size_t i=0;i < line.Length();i++)
	{
		wxChar token = line.GetChar(i);

		if (!bInHtmlTag && token == '<')
		{
			bInHtmlTag = true;

			tag.Clear();
		}
		else if (bInHtmlTag && token == '>')
		{
			if (tag.CmpNoCase(wxT("p")) == 0 ||
				tag.CmpNoCase(wxT("br")) == 0)
			{
				tmpLine = TransformHtml(tmpLine);

				if (!GetReadBookDoc()->IsEmptyLine(tmpLine))
				{
					pLines->Add(tmpLine);
				}

				tmpLine.Clear();
			}
			else if (tag.MakeLower().StartsWith(wxT("script ")))
			{
				m_bInScript = true;
			}
			else if (tag.CmpNoCase(wxT("/script")) == 0)
			{
				m_bInScript = false;
			}
			else if (tag.CmpNoCase(wxT("!--")) == 0)
			{
				m_bInScript = true;
			}
			else if (tag.CmpNoCase(wxT("--")) == 0)
			{
				m_bInScript = false;
			}

			bInHtmlTag = false;
		}
		else if (!bInHtmlTag && !m_bInScript)
		{
			tmpLine.Append(token);
		}
		else
		{
			tag.Append(token);
		}
	}

	tmpLine = TransformHtml(tmpLine);

	if (!GetReadBookDoc()->IsEmptyLine(tmpLine))
	{
		pLines->Add(tmpLine);
	}

	return pLines;
}

wxString CReadBookView::TransformHtml(const wxString & line)
{
	wxString tmpLine = line;

	tmpLine.Replace(wxT("&nbsp;"), wxT(" "), true);
	tmpLine.Replace(wxT("&quot;"), wxT("\""), true);
	tmpLine.Replace(wxT("&raquo;"), wxT(">>"), true);
	tmpLine.Replace(wxT("&amp;"), wxT("&"), true);
	tmpLine.Replace(wxT("&copy;"), wxT("&"), true);

#if wxUSE_UNICODE

	if (m_DisplayAs != wxReadBook::DisplayAsOriginal)
	{
		wxChar tmp1[2], tmp2[2];

		for(int i=0;i<gb2big5TableSize;i+=2)
		{
			tmp1[0] = m_DisplayAs == wxReadBook::DisplayAsSimplify ? gb2big5[i + 1] : gb2big5[i]; tmp1[1] = 0;
			tmp2[0] = m_DisplayAs == wxReadBook::DisplayAsSimplify ? gb2big5[i] : gb2big5[i + 1]; tmp2[1] = 0;

			tmpLine.Replace(tmp1, tmp2);
		}
	}
#endif

	return tmpLine;
}

void CReadBookView::OnMouseWheel(wxMouseEvent & event)
{
	wxInt32 nDeltaLine = event.GetWheelDelta() /
		(m_FontSize.GetHeight() + wxGetApp().GetPreference()->GetLineMargin());

	if (nDeltaLine <= 0)
		nDeltaLine = 1;

	bool doScroll = true;

	wxInt32 nCurrentLine = GetReadBookDoc()->GetCurrentLine();

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
		if (nCurrentLine == ScrollPage(nDeltaLine))
			doScroll = false;
	}
	else
	{
		if (nCurrentLine == ScrollPage(nDeltaLine))
			doScroll = false;
	}

	if (doScroll)
	{
		UpdateScrollPos();

		m_pCanvas->Refresh();
	}
}

bool CReadBookView::CalculateFontSize()
{
	wxClientDC dc(m_pCanvas);

	wxDC * pDC = &dc;

	const wxFont & pOldFont = pDC->GetFont();
	wxFont * pFont = wxGetApp().GetPreference()->GetFont();
	pDC->SetFont(*pFont);

	wxSize fontSize = dc.GetTextExtent(STANDARD_LINE);

	fontSize.SetWidth(fontSize.GetWidth() / STANDARD_LINE.Length() + 1);

#if wxUSE_UNICODE
	wxSize mbfontSize = dc.GetTextExtent(STANDARD_LINE_MB);

	mbfontSize.SetWidth(mbfontSize.GetWidth() / STANDARD_LINE_MB.Length() + 1);
#endif

	pDC->SetFont(pOldFont);

	if (fontSize.GetWidth() != m_FontSize.GetWidth() ||
		fontSize.GetHeight() != m_FontSize.GetHeight())
	{
		m_FontSize = fontSize;

#if wxUSE_UNICODE
		m_mbFontSize = mbfontSize;
#endif

		return true;
	}

	return false;
}

wxInt32 CReadBookView::ScrollPosToLine(wxInt32 nPos)
{
	return nPos;
}

void CReadBookView::SetDisplayAs(wxReadBook::DisplayAsEnum displayAs)
{
	m_DisplayAs = displayAs;

	m_pCanvas->Refresh();

	CReadBookDoc* pDoc = GetReadBookDoc();

	pDoc->UpdateDisplay(m_DisplayAs, m_ViewMode);
}