#ifndef READBOOKCANVAS_H_
#define READBOOKCANVAS_H_

#include <wx/docview.h>

class CReadBookCanvas: public wxWindow
{
public:
    CReadBookCanvas(wxView *v, wxFrame *frame, const wxPoint& pos, const wxSize& size, const long style);
    virtual ~CReadBookCanvas();
    
public:
    virtual void OnDraw(wxDC& dc);
    void OnPaint(wxPaintEvent& event);
    void OnScrollWin(wxScrollWinEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnSize(wxSizeEvent& event);
	void OnMouseWheel(wxMouseEvent & event);
	void OnLeftDown(wxMouseEvent & event);
    
    DECLARE_EVENT_TABLE()
    
private:
    wxView * m_pView;
    
public:
    wxView * GetView() const { return m_pView; }
    void SetView(wxView * pView) { m_pView = pView; }
};

#endif /*READBOOKCANVAS_H_*/
