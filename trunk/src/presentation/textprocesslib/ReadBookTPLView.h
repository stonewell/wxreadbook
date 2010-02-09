#pragma once

#include "../ReadBookView.h"

class CReadBookTPLView :
	public CReadBookView
{
    DECLARE_DYNAMIC_CLASS(CReadBookTPLView)

public:
	CReadBookTPLView(void);
	virtual ~CReadBookTPLView(void);

public:
    virtual void OnDraw(wxDC *dc);
    virtual void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    virtual void OnScrollWin(wxScrollWinEvent& event);
    virtual void OnMouseWheel(wxMouseEvent & event);
    virtual void OnKeyDown(wxKeyEvent& event);

	CReadBookTPLDoc * GetReadBookDoc() 
	{ return (CReadBookTPLDoc *)(GetDocument()); }

protected:
	virtual void CalculateViewSize();
	virtual void CalculateScrollSize(void);
	virtual void UpdateScrollPos(void);


	virtual void Recalculate();
private:
};

