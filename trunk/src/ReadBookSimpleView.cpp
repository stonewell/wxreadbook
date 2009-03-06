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

IMPLEMENT_DYNAMIC_CLASS(CReadBookSimpleView, CReadBookView)

CReadBookSimpleView::CReadBookSimpleView(void)
{
}

CReadBookSimpleView::~CReadBookSimpleView(void)
{
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


	pDC->SetFont(pOldFont);
	pDC->SetTextForeground(oldColor);
	pDC->SetBackground(oldBkColor);
}

void CReadBookSimpleView::CalculateViewSize()
{
	wxRect clientRect = m_pCanvas->GetClientRect();

	wxInt16 colMargin = wxGetApp().GetPreference()->GetColumnMargin();
	clientRect.Deflate(colMargin,0);

	CReadBookSimpleDoc * pDoc = GetReadBookDoc();

	m_nViewSize = pDoc->GetBufferSize();
}

wxInt32 CReadBookSimpleView::NormalizeScrollToLine(wxInt32 nCurrentLine)
{
	CReadBookSimpleDoc * pBufferedDoc = 
		(CReadBookSimpleDoc *)GetReadBookDoc();

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

	int pos = pBufferedDoc->RowToOffset(pBufferedDoc->GetCurrentLine());

	m_pCanvas->SetScrollbar(wxVERTICAL,
		pos,
		m_nPageSize,
		m_nViewSize);
}

void CReadBookSimpleView::UpdateScrollPos(void)
{
	CReadBookSimpleDoc * pBufferedDoc = 
		(CReadBookSimpleDoc *)GetReadBookDoc();

	m_pCanvas->SetScrollPos(wxVERTICAL, 
        pBufferedDoc->GetCurrentPosition());
}

wxInt32 CReadBookSimpleView::ScrollPosToLine(wxInt32 nPos)
{
	CReadBookSimpleDoc * pBufferedDoc = 
		(CReadBookSimpleDoc *)GetReadBookDoc();

	return pBufferedDoc->OffsetToRow(nPos);
}
