#pragma once

#include "ReadBookView.h"

class CReadBookPage;
class CReadBookChar;

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
    virtual void PreferenceChanged();

	CReadBookSimpleDoc * GetReadBookDoc() 
	{ return (CReadBookSimpleDoc *)(GetDocument()); }

protected:
	virtual void CalculateViewSize();
	virtual void CalculateScrollSize(void);
	virtual void UpdateScrollPos(void);
    virtual void CalculateLastCharOffset();
    virtual void CalculateFirstCharOffset();

	virtual wxFileOffset ScrollSimplePage(wxInt16 nDelta);
	virtual wxFileOffset ScrollSimpleLine(wxInt16 nDelta);
	virtual wxFileOffset ScrollToBeginPosition(wxFileOffset nPos, bool bUpdateFileInfo = true);
	virtual wxFileOffset ScrollToEndPosition(wxFileOffset nPos, bool bUpdateFileInfo = true);

    virtual wxFileOffset GetCurrentPosition();

    virtual void OnSize(wxSizeEvent& event);
    virtual wxRect GetClientRect();

	virtual CReadBookPage * CreateReadBookPage();
	virtual void ReleasePPChars();
	virtual void CreatePPChars();

	virtual void Recalculate();
private:
    CReadBookPage * m_pViewPage;
    wxFileOffset m_nFileEndPosition;
    wxFileOffset m_nFileBeginPosition;
	wxInt32 m_nCharsPerLine;
	CReadBookChar ** m_ppChars;
	wxInt32 m_nPageChars;
};
