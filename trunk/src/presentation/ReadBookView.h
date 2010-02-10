#ifndef READBOOKVIEW_H_
#define READBOOKVIEW_H_

#include "wx/docview.h"
#include "wx/datstrm.h"
#include "wx/arrstr.h"

class CReadBookDoc;

extern const wxString & STANDARD_LINE;

#if wxUSE_UNICODE
extern const wxString & STANDARD_LINE_MB;
#endif

class CReadBookView: public wxView
{
    DECLARE_DYNAMIC_CLASS(CReadBookView)

private:

public:
	struct LineNumberMapping
	{
		wxInt32 nDocLineCount;
		wxInt32 nViewLineCountBegin;
	};

	WX_DECLARE_HASH_MAP( wxInt32, LineNumberMapping, wxIntegerHash, wxIntegerEqual, CLineNumberMap);

    CReadBookView(void);
    virtual ~CReadBookView(void);

    virtual bool OnCreate(wxDocument *doc, long flags);
    virtual void OnDraw(wxDC *dc);
    virtual void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    virtual bool OnClose(bool deleteWindow = true);
    virtual void OnScrollWin(wxScrollWinEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnSize(wxSizeEvent& event);
	virtual void OnMouseWheel(wxMouseEvent & event);

    wxReadBook::ViewModeEnum GetViewMode() const { return m_ViewMode; }
    void SetViewMode(wxReadBook::ViewModeEnum ViewMode);

	wxReadBook::DisplayAsEnum GetDisplayAs() const { return m_DisplayAs; }
    void SetDisplayAs(wxReadBook::DisplayAsEnum displayAs);

	CReadBookDoc * GetReadBookDoc() 
	{ return (CReadBookDoc *)(GetDocument()); }

	void Recalculate(void);

    virtual void PreferenceChanged();
	virtual bool GoTo();

    DECLARE_EVENT_TABLE()

private:
	CLineNumberMap m_LineNumberMap;
    wxRect m_ClientSize;
	bool m_bInScript;
	wxReadBook::ViewModeEnum m_ViewMode;
	wxReadBook::DisplayAsEnum m_DisplayAs;

	const LineNumberMapping & ViewLineCountToDocLine(wxInt32 viewLine);

protected:
	wxInt16 m_nPageSize;

	wxInt32 m_nLastLineViewSize;
	wxInt32 m_nViewSize;

	wxSize m_FontSize;

#ifdef _UNICODE
	wxSize m_mbFontSize;
#endif

	virtual wxInt32 ScrollPage(wxInt16 nDelta);
	virtual wxInt32 ScrollLine(wxInt16 nDelta);
	virtual wxInt32 ScrollToLine(wxInt32 nLine);
	virtual wxInt32 ScrollToPosition(wxInt32 nPos)
	{
		return ScrollToLine(ScrollPosToLine(nPos));
	}

	bool CheckCharConsistence(const wxChar * pBuf, wxInt16 startIndex, wxInt16 count) const;

	virtual void CalculateScrollSize(void);

	virtual void CalculateViewSize();
	bool CalculateFontSize();

	wxInt16 GetViewLineCharSize(wxDC * pDC,
		const wxChar * pBuf, size_t length,
		size_t startIndex, wxInt16 defaultLineCharSize) const;

	virtual void UpdateScrollPos(void);

	wxArrayString * GetLineStrings(wxInt32 row);

	wxArrayString * StripHtml(const wxString & line);
	wxString TransformHtml(const wxString & line);

	virtual wxInt32 NormalizeScrollToLine(wxInt32 nLine);
	virtual wxInt32 ScrollPosToLine(wxInt32 nPos);
	virtual wxInt32 ScrollLineToPos(wxInt32 nLine);
	virtual wxInt32 GetCurrentLine();

	virtual wxInt32 GetCurrentPosition()
	{
		return ScrollLineToPos(GetCurrentLine());
	}

	virtual wxRect GetClientRect() const;
	virtual void SetVertScrollbar(int position, int thumbSize,
                              int range,
                              bool refresh = true);
	virtual void SetVertScrollPos(int pos, bool refresh = true);

	virtual void RefreshCanvas();

	virtual wxWindow * GetCanvas() const { return reinterpret_cast<wxWindow *>(GetCurrentCanvas()); }

	virtual void InitViewMode();
};
#endif /*READBOOKVIEW_H_*/

