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
#include "../ReadBookKeys.h"

#include "ReadBookTPLDoc.h"
#include "ReadBookTPLView.h"

IMPLEMENT_DYNAMIC_CLASS(CReadBookTPLView, CReadBookView)

CReadBookTPLView::CReadBookTPLView(void)
{
}

CReadBookTPLView::~CReadBookTPLView(void)
{
}

void CReadBookTPLView::OnDraw(wxDC *pDC)
{

}

void CReadBookTPLView::CalculateViewSize()
{
	CReadBookTPLDoc * pDoc = GetReadBookDoc();

	m_nViewSize = pDoc->GetBufferSize();
}

void CReadBookTPLView::CalculateScrollSize(void)
{
	CReadBookView::CalculateScrollSize();
}

void CReadBookTPLView::UpdateScrollPos(void)
{
	CReadBookView::UpdateScrollPos();
}

void CReadBookTPLView::OnScrollWin(wxScrollWinEvent& event)
{
	CReadBookView::OnScrollWin(event);
}

void CReadBookTPLView::OnMouseWheel(wxMouseEvent & event)
{
	CReadBookView::OnMouseWheel(event);
}

void CReadBookTPLView::OnUpdate(wxView *sender, wxObject *hint)
{
	CReadBookView::OnUpdate(sender, hint);
}

void CReadBookTPLView::OnKeyDown(wxKeyEvent& event)
{
	CReadBookView::OnKeyDown(event);
}

void CReadBookTPLView::Recalculate()
{
	CReadBookView::Recalculate();
}
