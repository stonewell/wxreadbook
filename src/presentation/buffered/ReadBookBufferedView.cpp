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

#include "../../ReadBookApp.h"
#include "../ReadBookDC.h"

#include "ReadBookBufferedDoc.h"
#include "ReadBookBufferedView.h"

IMPLEMENT_DYNAMIC_CLASS(CReadBookBufferedView, CReadBookView)

CReadBookBufferedView::CReadBookBufferedView(void)
{
	m_nLastLine = -1;
}

CReadBookBufferedView::~CReadBookBufferedView(void)
{
}

void CReadBookBufferedView::OnDraw(wxDC *pDC)
{
	CReadBookBufferedDoc* pDoc = (CReadBookBufferedDoc *)GetReadBookDoc();

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

	wxInt32 row = pDoc->GetCurrentLine();

	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();
	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();

	wxInt32 lineCount = row;

	wxInt32 nStartRow = -1;

	while(pDoc->RowToOffset(row) < pDoc->GetBufferSize())
	{
		wxArrayString * pLines = GetLineStrings(row);

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

void CReadBookBufferedView::CalculateViewSize()
{
	wxRect clientRect = GetClientRect();

	m_nLastLineViewSize = 1;

#if wxUSE_UNICODE
	m_nCharsPerLine = (clientRect.GetWidth() / m_mbFontSize.GetWidth()) * 2;
#else
	m_nCharsPerLine = clientRect.GetWidth() / m_FontSize.GetWidth();
#endif

	CReadBookBufferedDoc * pBufferedDoc =
		(CReadBookBufferedDoc *)GetReadBookDoc();

	pBufferedDoc->SetContentHelper(this);
	m_nViewSize = pBufferedDoc->GetBufferSize();
}

void CReadBookBufferedView::OnKeyDown(wxKeyEvent& event)
{
	bool doScroll = true;

	CReadBookBufferedDoc * pDoc =
		(CReadBookBufferedDoc*)GetReadBookDoc();

	wxInt32 nCurrentLine = pDoc->GetCurrentLine();

	switch(event.GetKeyCode())
	{
	case 'h':
	case 'H':
		pDoc->ShiftStream(-1);
		break;
	default:
		CReadBookView::OnKeyDown(event);
		return;
	}

	if (doScroll)
	{
		RefreshCanvas();
	}
}

wxInt32 CReadBookBufferedView::NormalizeScrollToLine(wxInt32 nCurrentLine)
{
	CReadBookBufferedDoc * pBufferedDoc =
		(CReadBookBufferedDoc *)GetReadBookDoc();

	m_nViewSize = pBufferedDoc->GetBufferSize();

	if (pBufferedDoc->GetBufferSize() <=
		pBufferedDoc->RowToOffset(nCurrentLine) + GetCharsPerLine())
	{
		if (m_nLastLine > 0)
		{
			return m_nLastLine;
		}

		return pBufferedDoc->OffsetToRow(pBufferedDoc->GetBufferSize() -
			GetCharsPerLine());
	}

	return nCurrentLine;
}

void CReadBookBufferedView::CalculateScrollSize(void)
{
	wxRect rect = GetClientRect();

	m_nPageSize = rect.GetHeight() /
		(m_FontSize.GetHeight() + wxGetApp().GetPreference()->GetLineMargin());

	if (m_nPageSize <= 0)
		m_nPageSize = 1;

	if (m_nViewSize < m_nPageSize)
		m_nPageSize = m_nViewSize;

	CReadBookBufferedDoc * pBufferedDoc =
		(CReadBookBufferedDoc *)GetReadBookDoc();

	int pos = pBufferedDoc->RowToOffset(pBufferedDoc->GetCurrentLine());

	SetVertScrollbar(pos,
		m_nPageSize,
		m_nViewSize);
}

void CReadBookBufferedView::UpdateScrollPos(void)
{
	CReadBookBufferedDoc * pBufferedDoc =
		(CReadBookBufferedDoc *)GetReadBookDoc();

	SetVertScrollPos(pBufferedDoc->RowToOffset(pBufferedDoc->GetCurrentLine()));
}

wxInt32 CReadBookBufferedView::ScrollPosToLine(wxInt32 nPos)
{
	CReadBookBufferedDoc * pBufferedDoc =
		(CReadBookBufferedDoc *)GetReadBookDoc();

	return pBufferedDoc->OffsetToRow(nPos);
}

wxInt32 CReadBookBufferedView:: ScrollLineToPos(wxInt32 nLine)
{
	CReadBookBufferedDoc * pBufferedDoc =
		(CReadBookBufferedDoc *)GetReadBookDoc();

	return pBufferedDoc->RowToOffset(nLine);
}


bool CReadBookBufferedView::CouldBeShowInSingleLine(const wxString & strLine) const
{
	wxRect clientRect = GetClientRect();

	CReadBookDC dc(GetCanvas());

	wxDC * pDC = &dc;

	const wxFont & pOldFont = pDC->GetFont();
	wxFont * pFont = wxGetApp().GetPreference()->GetFont();
	pDC->SetFont(*pFont);

	wxSize lineSize = dc.GetTextExtent(strLine);

	pDC->SetFont(pOldFont);

	return lineSize.GetX() <= clientRect.GetWidth();
}

const wxInt32 CReadBookBufferedView::GetCharsPerLine() const
{
	return m_nCharsPerLine;
}

