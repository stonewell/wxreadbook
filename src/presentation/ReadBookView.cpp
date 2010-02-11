#include "wx/wxprec.h"

#ifndef __BORLANDC__
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

#include "../ReadBookApp.h"
#include "../ReadBookMainFrm.h"
#include "../ReadBookCanvas.h"

#include "ReadBookDC.h"
#include "ReadBookDoc.h"
#include "ReadBookView.h"
#include "ReadBookKeys.h"

#if wxUSE_UNICODE
#include "../ns/nsDetector.h"
#include "../unicode/GBBig5Table.h"
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
m_ViewMode(wxReadBook::Text),
m_bInScript(false),
m_ClientSize(0,0,0,0),
m_DisplayAs(wxReadBook::DisplayAsOriginal),
m_nPageSize(0),
m_nLastLineViewSize(0),
m_nViewSize(0),
#ifdef _UNICODE
m_mbFontSize(0,0),
#endif
m_FontSize(0,0)
{
}

CReadBookView::~CReadBookView()
{
}

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool CReadBookView::OnCreate(wxDocument * WXUNUSED(doc), long WXUNUSED(flags) )
{
	GetCurrentCanvas()->SetView(this);

	// Associate the appropriate frame with this view.
	SetFrame(GetMainFrame());

	// Make sure the document manager knows that this is the
	// current view.
	Activate(true);

	Recalculate();

	return true;
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.

class B
{
public:
	typedef wxString::size_type (B::*LengthPtr)() const;
};

void CReadBookView::OnDraw(wxDC *pDC)
{
	CReadBookDoc* pDoc = (CReadBookDoc *)GetReadBookDoc();
	if (!pDoc)
		return;

	wxRect clientRect = GetClientRect();

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
				wxInt16 lineCharSize = 0;
				wxInt16 defualtLineCharSize =
					length * clientRect.GetWidth() / size.GetWidth() + 1;
				wxInt64 startIndex = 0;

				while(startIndex < length)
				{
					lineCharSize = GetViewLineCharSize(pDC,
						pBuf,length,startIndex,defualtLineCharSize);

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
	InitViewMode();

	m_bInScript = false;

	m_LineNumberMap.clear();

	Recalculate();

	RefreshCanvas();
}

// Clean up windows used for displaying the view.
bool CReadBookView::OnClose(bool WXUNUSED(deleteWindow))
{
	if (!GetReadBookDoc()->Close())
		return false;

	GetCurrentCanvas()->ClearBackground();
	GetCurrentCanvas()->SetView(NULL);

	wxString s(wxGetApp().GetAppName());

	GetMainFrame()->SetTitle(s);

	SetFrame((wxFrame *) NULL);

	Activate(false);

	m_LineNumberMap.clear();

	return true;
}

void CReadBookView::Recalculate(void)
{
	bool bFontChange = CalculateFontSize();

	wxRect tmpSize = GetClientRect();

	if (tmpSize.GetWidth() != m_ClientSize.GetWidth() || bFontChange || m_LineNumberMap.size() == 0)
	{
		CalculateViewSize();
	}

	CalculateScrollSize();

	m_ClientSize = GetClientRect();
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
	wxRect rect = GetClientRect();

	m_nPageSize = rect.GetHeight() /
		(m_FontSize.GetHeight() + wxGetApp().GetPreference()->GetLineMargin());

	if (m_nPageSize <= 0)
		m_nPageSize = 1;

	if (m_nViewSize < m_nPageSize)
		m_nPageSize = m_nViewSize;

	SetVertScrollbar(GetReadBookDoc()->GetCurrentLine(),
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
	wxRect clientRect = GetClientRect();

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

	while (size2.GetWidth() > clientRect.GetWidth() && lineCharSize > 1)
	{
		lineCharSize--;
		size2 = pDC->GetTextExtent(wxString(&pBuf[startIndex],lineCharSize));
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
		wxRect clientRect = GetClientRect();

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
	SetVertScrollPos(GetReadBookDoc()->GetCurrentLine());
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

		RefreshCanvas();
	}
}

void CReadBookView::OnKeyDown(wxKeyEvent& event)
{
	bool doScroll = true;

	wxInt32 nCurrentLine = GetReadBookDoc()->GetCurrentLine();

	int readbook_key = READBOOK_KEY_CODE(event.AltDown() ? 1 : 0,
		event.ControlDown() ? 1 : 0,
		event.ShiftDown() ? 1 : 0,
		event.GetKeyCode());

	switch(readbook_key)
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
	case 'F':
	case 'f':
		{
			wxCommandEvent eventCommand(wxEVT_COMMAND_MENU_SELECTED, IDM_FULL_SCREEN);

			GetMainFrame()->GetEventHandler()->ProcessEvent(eventCommand);
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
			wxCommandEvent eventCommand(wxEVT_COMMAND_MENU_SELECTED, IDM_HIDE);

			GetMainFrame()->GetEventHandler()->ProcessEvent(eventCommand);
			break;
		}
#endif
	case ReadBOOK_Key_CTRL_G:
		{
			GoTo();
			doScroll = false;
		}
		break;
	default:
		doScroll = false;
		event.Skip();
		break;
	}

	if (doScroll)
	{
		UpdateScrollPos();

		RefreshCanvas();
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

	RefreshCanvas();
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

		RefreshCanvas();
	}
}

bool CReadBookView::CalculateFontSize()
{
	CReadBookDC dc(GetCurrentCanvas());

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

wxInt32 CReadBookView:: ScrollLineToPos(wxInt32 nLine)
{
	return nLine;
}

void CReadBookView::SetDisplayAs(wxReadBook::DisplayAsEnum displayAs)
{
	m_DisplayAs = displayAs;

	RefreshCanvas();

	CReadBookDoc* pDoc = GetReadBookDoc();

	pDoc->UpdateDisplay(m_DisplayAs, m_ViewMode);
}

void CReadBookView::PreferenceChanged()
{
    Recalculate();
}

bool CReadBookView::GoTo()
{
	bool doScroll = true;

	double position = GetCurrentPosition();

	position = m_nViewSize == 0 ? 0 : position * 100/ (double)m_nViewSize;

	wxString strDefaultValue = wxString::Format(wxT("%.2f"), position);

	wxTextEntryDialog dialog(GetMainFrame(),
							 wxT("Please Input the position (0-100%):"),
							 wxT("Goto any place inside the document"),
							 strDefaultValue,
							 wxOK | wxCANCEL);

	if (dialog.ShowModal() == wxID_OK)
	{
		if (dialog.GetValue().ToDouble(&position) && position <= 100 && position >=0)
		{
			wxInt32 pos = m_nViewSize * position / 100;

			ScrollToPosition(pos);
		}
	}
	else
	{
		doScroll = false;
	}

	if (doScroll)
	{
		UpdateScrollPos();

		RefreshCanvas();
	}

	return doScroll;
}

wxInt32 CReadBookView::GetCurrentLine()
{
	return GetReadBookDoc()->GetCurrentLine();
}

wxRect CReadBookView::GetClientRect() const
{
	wxWindow * pCanvas = GetCurrentCanvas();

	if (!pCanvas)
		return wxRect(0,0,0,0);

	wxRect rect = pCanvas->GetClientRect();

	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();

	rect.Deflate(colMargin + 15,0);

	rect.SetWidth(rect.GetWidth());

	return rect;
}

void CReadBookView::SetVertScrollbar(int position, int thumbSize,
                              int range,
                              bool refresh)
{
	wxWindow * pCanvas = GetCurrentCanvas();

	if (!pCanvas) return;

	pCanvas->SetScrollbar(wxVERTICAL,
		position,
		thumbSize,
		range, refresh);
}

void CReadBookView::SetVertScrollPos(int pos, bool refresh)
{
	wxWindow * pCanvas = GetCurrentCanvas();
	if (!pCanvas) return;

	pCanvas->SetScrollPos(wxVERTICAL,
		pos,
		refresh);
}

void CReadBookView::RefreshCanvas()
{
	wxWindow * pCanvas = GetCurrentCanvas();
	if (!pCanvas) return;

	pCanvas->Refresh();
}

void CReadBookView::InitViewMode()
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
}