#pragma once

#include "ReadBookView.h"

class CReadBookSimpleView :
	public CReadBookView
{
    DECLARE_DYNAMIC_CLASS(CReadBookSimpleView)

public:
	CReadBookSimpleView(void);
	virtual ~CReadBookSimpleView(void);

public:
    virtual void OnDraw(wxDC *dc);

	CReadBookSimpleDoc * GetReadBookDoc() 
	{ return (CReadBookSimpleDoc *)(GetDocument()); }

protected:
	virtual void CalculateViewSize();
	virtual wxInt32 NormalizeScrollToLine(wxInt32 nCurrentLine);
	virtual void CalculateScrollSize(void);
	virtual void UpdateScrollPos(void);
	virtual wxInt32 ScrollPosToLine(wxInt32 nPos);
};
