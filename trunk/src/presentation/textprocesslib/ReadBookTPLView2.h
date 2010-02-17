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

	virtual TextProcess::View::IViewLine * GetPreviousLine(TextProcess::View::IViewLine * pCurViewLine);
	virtual TextProcess::View::IViewLine * GetNextLine(TextProcess::View::IViewLine * pCurViewLine);

private:
	void StartViewLineBuilder();
	void StopViewLineBuilder();

	std::auto_ptr<TextProcess::View::IViewLineManager> m_pViewLineManager;
	std::auto_ptr<TextProcess::View::IViewLineBuilder> m_pViewLineBuilderPrev;
	std::auto_ptr<TextProcess::View::IViewLineBuilder> m_pViewLineBuilderNext;
	std::auto_ptr<wxRect> m_pClientRect;
	bool m_bViewLineBuilding;
	TextProcess::Utils::IReadWriteLock * m_pLineManagerLock;
	TextProcess::View::IViewLine * m_pViewLine;
};

