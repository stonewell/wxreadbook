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

    int pos = pBufferedDoc->GetCurrentPosition();

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

void CReadBookSimpleView::OnScrollWin(wxScrollWinEvent& event)
{
	bool doScroll = true;

	wxFileOffset nCurrentPos = GetReadBookDoc()->GetCurrentPosition();

	if (event.GetEventType() == wxEVT_SCROLLWIN_TOP)
	{
		if (nCurrentPos == ScrollToPosition(0))
		{
			doScroll = false;
		}
	}
	else if (event.GetEventType() == wxEVT_SCROLLWIN_BOTTOM)
	{
		if (nCurrentPos == ScrollToPosition(m_nViewSize))
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

	wxFileOffset nCurrentLine = GetReadBookDoc()->GetCurrentPosition();

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
		if (nCurrentLine == ScrollSimplePage(nDeltaLine))
			doScroll = false;
	}
	else
	{
		if (nCurrentLine == ScrollSimplePage(nDeltaLine))
			doScroll = false;
	}

	if (doScroll)
	{
		UpdateScrollPos();

		m_pCanvas->Refresh();
	}
}

wxFileOffset CReadBookSimpleView::ScrollSimplePage(wxInt16 nDelta)
{
    return 0;
}

wxFileOffset CReadBookSimpleView::ScrollSimpleLine(wxInt16 nDelta)
{
    return 0;
}

wxFileOffset CReadBookSimpleView::ScrollToPosition(wxFileOffset nDelta)
{
    return 0;
}

