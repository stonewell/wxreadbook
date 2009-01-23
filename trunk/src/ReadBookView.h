#ifndef READBOOKVIEW_H_
#define READBOOKVIEW_H_

#include "wx/docview.h"
#include "wx/datstrm.h"
#include "wx/arrstr.h"

class CReadBookCanvas;
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

	enum ViewModeEnum
	{
		Html,
		Text
	};

	WX_DECLARE_HASH_MAP( wxInt32, LineNumberMapping, wxIntegerHash, wxIntegerEqual, CLineNumberMap);

    CReadBookView(void);
    virtual ~CReadBookView(void);

    bool OnCreate(wxDocument *doc, long flags);
    virtual void OnDraw(wxDC *dc);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = true);
    void OnScrollWin(wxScrollWinEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    void OnSize(wxSizeEvent& event);
	void OnMouseWheel(wxMouseEvent & event);

    ViewModeEnum GetViewMode() const { return m_ViewMode; }
    void SetViewMode(ViewModeEnum ViewMode);

    bool IsDisplayChineseSimplify() const { return m_bDisplayChineseSimplify; }
    void SetDisplayChineseSimplify(bool bDisplayChineseSimplify);

	CReadBookDoc * GetReadBookDoc() 
	{ return (CReadBookDoc *)(GetDocument()); }

	void Recalculate(void);

    DECLARE_EVENT_TABLE()

protected:
	wxFrame * m_pFrame;

    CReadBookCanvas * m_pCanvas;

	CLineNumberMap m_LineNumberMap;
	wxInt16 m_nPageSize;

	wxInt32 m_nLastLineViewSize;
	wxInt32 m_nViewSize;

	wxSize m_FontSize;

	ViewModeEnum m_ViewMode;

	bool m_bInScript;

    wxSize m_ClientSize;

#ifdef _UNICODE
	wxSize m_mbFontSize;
#endif

	bool m_bDisplayChineseSimplify;

	wxInt16 GetPageSize(void);
	wxInt16 SetPageSize(wxInt16 nPageSize);

	wxInt32 ScrollPage(wxInt16 nDelta);
	wxInt32 ScrollLine(wxInt16 nDelta);

	const LineNumberMapping & ViewLineCountToDocLine(wxInt32 viewLine);

	bool CheckCharConsistence(const wxChar * pBuf, wxInt16 startIndex, wxInt16 count) const;

	virtual void CalculateScrollSize(void);

	virtual void CalculateViewSize();
	bool CalculateFontSize();

	wxInt16 GetViewLineCharSize(wxDC * pDC,
		const wxChar * pBuf, size_t length,
		size_t startIndex, wxInt16 defaultLineCharSize) const;

	wxInt32 ScrollToLine(wxInt32 nPos);

	virtual void UpdateScrollPos(void);

	wxArrayString * GetLineStrings(wxInt32 row);

	wxArrayString * StripHtml(const wxString & line);
	wxString TransformHtml(const wxString & line);

	virtual wxInt32 NormalizeScrollToLine(wxInt32 nLine);
	virtual wxInt32 ScrollPosToLine(wxInt32 nPos);
};
#endif /*READBOOKVIEW_H_*/
