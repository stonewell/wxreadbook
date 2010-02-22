#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/filesys.h"

#include "../../ReadBookApp.h"
#include "../ReadBookDC.h"
#include "../ReadBookKeys.h"

#include "ReadBookTPLDoc.h"
#include "ReadBookTPLView.h"

class CViewBuilderRunnable :
	public PortableThread::IPortableRunnable
{
public:
	unsigned long Run(void * pArgument)
	{
		TextProcess::View::IViewLineBuilder * pLineBuilder =
			reinterpret_cast<TextProcess::View::IViewLineBuilder *>(pArgument);

		return pLineBuilder->BuildLines();
	}
};

IMPLEMENT_DYNAMIC_CLASS(CReadBookTPLView, CReadBookView)

CReadBookTPLView::CReadBookTPLView(void) :
m_BuildPrevThread(new CViewBuilderRunnable())
,m_BuildNextThread(new CViewBuilderRunnable())
,m_pViewLine(NULL)
,m_bViewLineBuilding(false)
,m_pLineManagerLock(TextProcess::Utils::IReadWriteLock::CreateReadWriteLock())
{
}

CReadBookTPLView::~CReadBookTPLView(void)
{
	StopViewLineBuilder();

	delete m_BuildPrevThread.GetPortableRunnable();
	delete m_BuildNextThread.GetPortableRunnable();
	delete m_pLineManagerLock;
}

void CReadBookTPLView::OnDraw(wxDC *pDC)
{
TPL_PRINTF("OnDraw Enter\n");
	CReadBookTPLDoc* pDoc = GetReadBookDoc();

	if (!pDoc || !pDoc->IsDocumentLoading() || !m_bViewLineBuilding)
		return;

	wxRect clientRect = GetClientRect();

	const wxFont & pOldFont = pDC->GetFont();

	wxFont * pNewFont = wxGetApp().GetPreference()->GetFont();
	pDC->SetFont(*pNewFont);

	const wxColour & oldColor = pDC->GetTextForeground();

	pDC->SetTextForeground(wxGetApp().GetPreference()->GetTextColor());

	const wxBrush & oldBkColor = pDC->GetBackground();

	pDC->SetBackground(wxBrush(wxGetApp().GetPreference()->GetBackgroundColor()));

	wxInt16 y = 0;

	wxInt32 currentLine = pDoc->GetCurrentLine();

	wxInt16 lineMargin = wxGetApp().GetPreference()->GetLineMargin();

	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);

	if (m_pViewLine == NULL)
	{
		std::auto_ptr<TextProcess::View::IViewLineMatcher> pMatcher(TextProcess::View::CViewObjectFactory::CreateLineMatcher(currentLine, 0));

		m_pViewLine = 
			m_pViewLineManager->FindLine(pMatcher.get());

		if (m_pViewLine == NULL)
		{
			pMatcher->SetDocumentLineOffset(0);
			pMatcher->SetViewLineOffset(0);
		}
		else
		{
			m_pViewLine->AccessLine();
			pMatcher->SetViewLineOffset(currentLine - m_pViewLine->GetDocumentLine()->GetOffset());
		}

		m_pViewLine = 
			m_pViewLineManager->FindLine(pMatcher.get());
	}

	if (m_pViewLine == NULL)
		return;

	TextProcess::View::IViewLine * pViewLine = m_pViewLine;

	for (int i=0; i < m_nPageSize; i++)
	{
		wxString line = pViewLine->GetDocumentLine()->GetData(pViewLine->GetOffset(), pViewLine->GetLength());

		pDC->DrawText(line, clientRect.GetLeft(), y);
		
		pViewLine->AccessLine();

		y += m_FontSize.GetY();

		//Line Margin
		y+=lineMargin;

		if (y >= clientRect.GetHeight())
			break;

		pViewLine = m_pViewLineManager->GetNextLine(pViewLine);

		if (pViewLine == NULL)
			break;
	}//for each line

	pDC->SetFont(pOldFont);
	pDC->SetTextForeground(oldColor);
	pDC->SetBackground(oldBkColor);
TPL_PRINTF("OnDraw Done\n");
}

void CReadBookTPLView::CalculateViewSize()
{
	CReadBookTPLDoc * pDoc = GetReadBookDoc();

	m_nViewSize = pDoc->GetBufferSize();
}

void CReadBookTPLView::Recalculate()
{
	CReadBookView::Recalculate();

	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);
	StopViewLineBuilder();
	StartViewLineBuilder();
}

wxInt32 CReadBookTPLView::ScrollLine(wxInt16 nDelta)
{
TPL_PRINTF("ScrollLine Enter\n");
	if (!GetReadBookDoc()->IsDocumentLoading() || !m_bViewLineBuilding)
		return GetReadBookDoc()->GetCurrentLine();

	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);
	if (nDelta < 0)
	{
		for(int i = 0; i > nDelta; i--)
		{
			TextProcess::View::IViewLine * pViewLine = 
				m_pViewLineManager->GetPrevLine(m_pViewLine);

			if (pViewLine == NULL) break;

			pViewLine->AccessLine();
			m_pViewLine = pViewLine;
		}
	}
	else if (nDelta > 0)
	{
TPL_PRINTF("ScrollLine Next Start\n");
		for(int i = 0; i < nDelta; i++)
		{
TPL_PRINTF("ScrollLine Next 1\n");
			TextProcess::View::IViewLine * pViewLine = 
				m_pViewLineManager->GetNextLine(m_pViewLine);

TPL_PRINTF("ScrollLine Next 2\n");
			if (pViewLine == NULL) break;

TPL_PRINTF("ScrollLine Next 3\n");
			pViewLine->AccessLine();

TPL_PRINTF("ScrollLine Next 4\n");
			m_pViewLine = pViewLine;
		}
TPL_PRINTF("ScrollLine Next Done\n");
	}

	wxFileOffset offset = m_pViewLine->GetDocumentLine()->GetOffset();
	GetReadBookDoc()->SetCurrentLine(offset + m_pViewLine->GetOffset());

TPL_PRINTF("ScrollLine Leave\n");
	return GetReadBookDoc()->GetCurrentLine();
}

wxInt32 CReadBookTPLView::ScrollToLine(wxInt32 nLine)
{
	if (!GetReadBookDoc()->IsDocumentLoading() || !m_bViewLineBuilding)
		return GetReadBookDoc()->GetCurrentLine();

	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);
	std::auto_ptr<TextProcess::View::IViewLineMatcher> pMatcher(TextProcess::View::CViewObjectFactory::CreateLineMatcher(nLine, 0));

	TextProcess::View::IViewLine * pViewLine = NULL;

	for(int i=0; i < 3; i++)
	{
		pViewLine = m_pViewLineManager->FindLine(pMatcher.get(), 0);

		if (pViewLine != NULL)
			break;
		else
		{
#ifdef _WIN32
			Sleep(100);
#elif HAVE_NANOSLEEP
			struct timespec req;
			req.tv_sec = 0;
			req.tv_nsec = 100 * 1000 * 1000;
			nanosleep(&req, NULL);
#elif HAVE_USLEEP
			usleep(100 * 1000);
#endif
		}
	}

	if (pViewLine == NULL)
	{
		StopViewLineBuilder();
		GetReadBookDoc()->ScrollDocumentTo(nLine);
		StartViewLineBuilder();
		pViewLine = m_pViewLineManager->FindLine(pMatcher.get());
	}

	if (pViewLine == NULL)
		return GetReadBookDoc()->GetCurrentLine();

	pViewLine->AccessLine();

	wxInt32 nViewLineOffset = 
		pViewLine->GetDocumentLine()->GetDecodedLength() *
		((nLine - pViewLine->GetDocumentLine()->GetOffset()) / pViewLine->GetDocumentLine()->GetLength());

	pMatcher->SetViewLineOffset(nViewLineOffset);

	pViewLine = 
		m_pViewLineManager->FindLine(pMatcher.get());

	if (pViewLine != NULL)
		m_pViewLine = pViewLine;
	else
		m_pViewLine = NULL;

	if (m_pViewLine != NULL)
	{
		m_pViewLine->AccessLine();
		wxFileOffset offset = m_pViewLine->GetDocumentLine()->GetOffset();
		GetReadBookDoc()->SetCurrentLine(offset + m_pViewLine->GetOffset());
	}

	return GetReadBookDoc()->GetCurrentLine();
}

void CReadBookTPLView::StartViewLineBuilder()
{
	if (!GetReadBookDoc()->IsDocumentLoading())
		return;

	wxFileOffset docPos = GetReadBookDoc()->GetCurrentLine();

	m_pViewLineManager.reset(TextProcess::View::CViewObjectFactory::CreateLineManager());
	m_pViewLine = NULL;

	m_pViewLineBuilderPrev.reset(TextProcess::View::CViewObjectFactory::CreateLineBuilder());

	m_pGraphic.reset(new CViewBuilderGraphic(this));

	m_pClientRect.reset(new wxRect(GetClientRect()));

	m_pViewLineBuilderPrev->SetBuilderDirection(TextProcess::Prev);
	m_pViewLineBuilderPrev->SetClientArea(m_pClientRect.get());
	m_pViewLineBuilderPrev->SetDocumentLineManager(GetReadBookDoc()->GetDocumentLineManager());
	m_pViewLineBuilderPrev->SetDocumentLineOffset(docPos);
	m_pViewLineBuilderPrev->SetGraphics(m_pGraphic.get());
	m_pViewLineBuilderPrev->SetViewFont(wxGetApp().GetPreference()->GetFont());
	m_pViewLineBuilderPrev->SetViewLineManager(m_pViewLineManager.get());
	m_pViewLineBuilderPrev->SetViewLineOffset(0);
	m_pViewLineBuilderPrev->SetBuildLineCount(m_nPageSize * 3);

	m_BuildPrevThread.SetRunningArgument(m_pViewLineBuilderPrev.get());
	m_BuildPrevThread.Start();

	m_pViewLineBuilderNext.reset(TextProcess::View::CViewObjectFactory::CreateLineBuilder());

	m_pViewLineBuilderNext->SetBuilderDirection(TextProcess::Next);
	m_pViewLineBuilderNext->SetClientArea(m_pClientRect.get());
	m_pViewLineBuilderNext->SetDocumentLineManager(GetReadBookDoc()->GetDocumentLineManager());
	m_pViewLineBuilderNext->SetDocumentLineOffset(docPos);
	m_pViewLineBuilderNext->SetGraphics(m_pGraphic.get());
	m_pViewLineBuilderNext->SetViewFont(wxGetApp().GetPreference()->GetFont());
	m_pViewLineBuilderNext->SetViewLineManager(m_pViewLineManager.get());
	m_pViewLineBuilderNext->SetViewLineOffset(0);
	m_pViewLineBuilderNext->SetBuildLineCount(m_nPageSize * 3);

	m_BuildNextThread.SetRunningArgument(m_pViewLineBuilderNext.get());
	m_BuildNextThread.Start();

	m_bViewLineBuilding = true;
}

void CReadBookTPLView::StopViewLineBuilder()
{
	m_bViewLineBuilding = false;

	if (m_pViewLineBuilderPrev.get() != NULL)
	{
		m_pViewLineBuilderPrev->Cancel();
		if (!m_BuildPrevThread.Join(3000))
			m_BuildPrevThread.Abort();
	}

	if (m_pViewLineBuilderNext.get() != NULL)
	{
		m_pViewLineBuilderNext->Cancel();
		if (!m_BuildNextThread.Join(3000))
			m_BuildNextThread.Abort();
	}
}

