#pragma once

#include "../ReadBookView.h"
#include "../ReadBookDC.h"
#include <memory>
#include "TextProcess.h"
#include "PortableThread.h"

class CViewBuilderGraphic :
	public TextProcess::View::IViewLineBuilderGraphic
{
public:
	CViewBuilderGraphic(CReadBookView * pView) : m_pView(pView)
	, m_pSection(TextProcess::Utils::ICriticalSection::CreateCriticalSection())
	{ }

	virtual ~CViewBuilderGraphic() 
	{
		delete m_pSection;
	}

	virtual void GetTextExtent(const wxString& string,
                       wxCoord *x, wxCoord *y,
                       wxFont *theFont = NULL) const
	{
		TextProcess::Utils::CCriticalSectionAccessor a(m_pSection);
		CReadBookDC dc(m_pView->GetCanvas());
		dc.GetTextExtent(string, x, y, NULL, NULL, theFont);
	}

private:
	CReadBookView * m_pView;
	TextProcess::Utils::ICriticalSection * m_pSection;
};

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
	virtual void UpdateScrollPos(void);
	virtual wxInt32 ScrollPosToLine(wxInt32 nPos);
	virtual wxInt32 ScrollLineToPos(wxInt32 nLine);

private:
	void StartViewLineBuilder();
	void StopViewLineBuilder();

	std::shared_ptr<TextProcess::View::IViewLineManager> m_pViewLineManager;
	std::shared_ptr<TextProcess::View::IViewLineBuilder> m_pViewLineBuilderPrev;
	std::shared_ptr<TextProcess::View::IViewLineBuilder> m_pViewLineBuilderNext;
	PortableThread::CPortableThread m_BuildPrevThread;
	PortableThread::CPortableThread m_BuildNextThread;
	TextProcess::View::IViewLine * m_pViewLine;
	std::shared_ptr<CViewBuilderGraphic> m_pGraphic;
	std::shared_ptr<wxRect> m_pClientRect;
	bool m_bViewLineBuilding;
	TextProcess::Utils::IReadWriteLock * m_pLineManagerLock;
};

