#pragma once

#include "../ReadBookView.h"
#include "../ReadBookDC.h"
#include <memory>
#include "TextProcess.h"

class CReadBookTPLView2 :
	public CReadBookView
{
    DECLARE_DYNAMIC_CLASS(CReadBookTPLView2)

public:
	CReadBookTPLView2(void);
	virtual ~CReadBookTPLView2(void);

public:
    virtual void OnDraw(wxDC *dc);

	CReadBookTPLDoc2 * GetReadBookDoc() 
	{ return (CReadBookTPLDoc2 *)(GetDocument()); }

protected:
	virtual void CalculateViewSize();
	virtual void Recalculate();
	virtual wxInt32 ScrollLine(wxInt16 nDelta);
	virtual wxInt32 ScrollToLine(wxInt32 nLine);
	virtual void UpdateScrollPos(void);
	virtual wxInt32 ScrollPosToLine(wxInt32 nPos);
	virtual wxInt32 ScrollLineToPos(wxInt32 nLine);

    virtual void OnScrollWin(wxScrollWinEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnSize(wxSizeEvent& event);
	virtual void OnMouseWheel(wxMouseEvent & event);

	virtual TextProcess::View::IViewLine * GetPreviousLine(TextProcess::View::IViewLine * pCurViewLine);
	virtual TextProcess::View::IViewLine * GetNextLine(TextProcess::View::IViewLine * pCurViewLine);

private:
	void StartViewLineBuilder();
	void StartViewLineBuilder(wxFileOffset docOffset, wxFileOffset viewOffset);
	void StopViewLineBuilder();

	std::auto_ptr<TextProcess::View::IViewLineManager> m_pViewLineManager;
	std::auto_ptr<TextProcess::View::IViewLineBuilder> m_pViewLineBuilderPrev;
	std::auto_ptr<TextProcess::View::IViewLineBuilder> m_pViewLineBuilderNext;
	std::auto_ptr<wxRect> m_pClientRect;
	bool m_bViewLineBuilding;
	TextProcess::Utils::IReadWriteLock * m_pLineManagerLock;
	TextProcess::View::IViewLine * m_pViewLine;
};

