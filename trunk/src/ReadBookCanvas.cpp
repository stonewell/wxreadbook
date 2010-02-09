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

#include <wx/dcclient.h>

#include "ReadBookApp.h"
#include "ReadBookCanvas.h"
#include "presentation/ReadBookDoc.h"
#include "presentation/ReadBookView.h"

BEGIN_EVENT_TABLE(CReadBookCanvas, wxWindow)
	EVT_PAINT(CReadBookCanvas::OnPaint)
	EVT_SCROLLWIN(CReadBookCanvas::OnScrollWin)
	EVT_KEY_DOWN(CReadBookCanvas::OnKeyDown)
	EVT_SIZE(CReadBookCanvas::OnSize)
	EVT_MOUSEWHEEL(CReadBookCanvas::OnMouseWheel)
END_EVENT_TABLE()

CReadBookCanvas::~CReadBookCanvas()
{
}

// Define a constructor for my canvas
CReadBookCanvas::CReadBookCanvas(wxView *v, 
	wxFrame *frame, const wxPoint& pos, const wxSize& size, const long style):
		wxWindow(frame, wxID_ANY, pos, size, style | wxFULL_REPAINT_ON_RESIZE | wxVSCROLL)
{
    m_pView = v;
}

// Define the repainting behaviour
void CReadBookCanvas::OnDraw(wxDC& dc)
{
    if (m_pView)
        m_pView->OnDraw(& dc);
}

void CReadBookCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc(this);
	
	OnDraw(dc);
}

void CReadBookCanvas::OnScrollWin(wxScrollWinEvent& event)
{
	if (m_pView)
		((CReadBookView *)m_pView)->OnScrollWin(event);
}

void CReadBookCanvas::OnKeyDown(wxKeyEvent& event)
{
	if (m_pView)
		((CReadBookView *)m_pView)->OnKeyDown(event);
}

void CReadBookCanvas::OnSize(wxSizeEvent& event)
{
	if (m_pView)
		((CReadBookView *)m_pView)->OnSize(event);
}

void CReadBookCanvas::OnMouseWheel(wxMouseEvent & event)
{
	if (m_pView)
		((CReadBookView *)m_pView)->OnMouseWheel(event);
}
