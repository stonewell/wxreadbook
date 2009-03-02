#pragma once

class CReadBookSimpleView :
	public wxView
{
    DECLARE_DYNAMIC_CLASS(CReadBookSimpleView)

public:
	CReadBookSimpleView(void);
	virtual ~CReadBookSimpleView(void);

public:
    virtual void OnDraw(wxDC *dc);
    virtual void OnKeyDown(wxKeyEvent& event);

    bool OnCreate(wxDocument *doc, long flags);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = true);
    void OnScrollWin(wxScrollWinEvent& event);
    void OnSize(wxSizeEvent& event);
	void OnMouseWheel(wxMouseEvent & event);

    wxReadBook::ViewModeEnum GetViewMode() const { return m_ViewMode; }
    void SetViewMode(wxReadBook::ViewModeEnum ViewMode);

	wxReadBook::DisplayAsEnum GetDisplayAs() const { return m_DisplayAs; }
    void SetDisplayAs(wxReadBook::DisplayAsEnum displayAs);

	CReadBookSimplDoc * GetReadBookDoc() 
	{ return (CReadBookSimpleDoc *)(GetDocument()); }

	void Recalculate(void);

    DECLARE_EVENT_TABLE()

protected:
	wxInt32 m_nLastLine;
	wxInt32 m_nCharsPerLine;

	virtual void CalculateViewSize();
	virtual wxInt32 NormalizeScrollToLine(wxInt32 nCurrentLine);
	virtual void CalculateScrollSize(void);
	virtual void UpdateScrollPos(void);
	virtual wxInt32 ScrollPosToLine(wxInt32 nPos);
};
