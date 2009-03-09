#pragma once

#include "ReadBookView.h"

class CReadBookPage;

class CReadBookSimpleView :
	public CReadBookView
{
    DECLARE_DYNAMIC_CLASS(CReadBookSimpleView)

public:
	CReadBookSimpleView(void);
	virtual ~CReadBookSimpleView(void);

public:
    virtual void OnDraw(wxDC *dc);
    virtual void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    virtual void OnScrollWin(wxScrollWinEvent& event);
    virtual void OnMouseWheel(wxMouseEvent & event);
    virtual void OnKeyDown(wxKeyEvent& event);

	CReadBookSimpleDoc * GetReadBookDoc() 
	{ return (CReadBookSimpleDoc *)(GetDocument()); }

protected:
	virtual void CalculateViewSize();
	virtual void CalculateScrollSize(void);
	virtual void UpdateScrollPos(void);

	virtual wxFileOffset ScrollSimplePage(wxInt16 nDelta);
	virtual wxFileOffset ScrollSimpleLine(wxInt16 nDelta);
	virtual wxFileOffset ScrollToPosition(wxFileOffset nPos);
    virtual wxFileOffset ScrollToLastPage();

    virtual wxFileOffset GetCurrentPosition();

    virtual void CalculateLastCharOffset();
    virtual void OnSize(wxSizeEvent& event);
private:
    CReadBookPage * m_pViewPage;
    wxFileOffset m_nFileEndPosition;
};
