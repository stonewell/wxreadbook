#pragma once

#include "../ReadBookView.h"
#include "../ReadBookDC.h"
#include <memory>
#include "TextProcess.h"
#include "PortableThread.h"

class CReadBookTPLView :
	public CReadBookView
{
    DECLARE_DYNAMIC_CLASS(CReadBookTPLView)

public:
	CReadBookTPLView(void);
	virtual ~CReadBookTPLView(void);

public:
    virtual void OnDraw(wxDC *dc);

	CReadBookTPLDoc * GetReadBookDoc() 
	{ return (CReadBookTPLDoc *)(GetDocument()); }

protected:
	virtual void CalculateViewSize();
	virtual void Recalculate();
	virtual wxInt32 ScrollLine(wxInt16 nDelta);
	virtual wxInt32 ScrollToLine(wxInt32 nLine);

private:
	void StartViewLineBuilder();
	void StopViewLineBuilder();

	std::auto_ptr<TextProcess::View::IViewLineManager> m_pViewLineManager;
	std::auto_ptr<TextProcess::View::IViewLineBuilder> m_pViewLineBuilderPrev;
	std::auto_ptr<TextProcess::View::IViewLineBuilder> m_pViewLineBuilderNext;
	PortableThread::CPortableThread m_BuildPrevThread;
	PortableThread::CPortableThread m_BuildNextThread;
	TextProcess::View::IViewLine * m_pViewLine;
	std::auto_ptr<CReadBookDC> m_pClientDC;
	std::auto_ptr<wxRect> m_pClientRect;
	bool m_bViewLineBuilding;
};

