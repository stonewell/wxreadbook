#pragma once

#include "ReadBookView.h"

class CReadBookBufferedView :
	public CReadBookView
{
    DECLARE_DYNAMIC_CLASS(CReadBookBufferedView)

public:
	CReadBookBufferedView(void);
	virtual ~CReadBookBufferedView(void);

public:
	virtual void OnDraw(wxDC *dc);
    virtual void OnKeyDown(wxKeyEvent& event);

protected:
	wxInt32 m_nLastLine;

	virtual void CalculateViewSize();
	virtual wxInt32 NormalizeScrollToLine(wxInt32 nCurrentLine);
	virtual void CalculateScrollSize(void);
	virtual void UpdateScrollPos(void);
	virtual wxInt32 ScrollPosToLine(wxInt32 nPos);
};
