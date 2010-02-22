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

#include "ReadBookTPLDoc2.h"
#include "ReadBookTPLView2.h"

class CViewBuilderGraphic2 :
	public TextProcess::View::IViewLineBuilderGraphic
{
public:
	CViewBuilderGraphic2(CReadBookDC * pDC) : m_pDC(pDC)
	{ }

	virtual ~CViewBuilderGraphic2() 
	{
	}

	virtual void GetTextExtent(const wxString& string,
                       wxCoord *x, wxCoord *y,
                       wxFont *theFont = NULL) const
	{
		m_pDC->GetTextExtent(string, x, y, NULL, NULL, theFont);
	}

private:
	CReadBookDC * m_pDC;
};


IMPLEMENT_DYNAMIC_CLASS(CReadBookTPLView2, CReadBookView)

CReadBookTPLView2::CReadBookTPLView2(void) :
m_pViewLine(NULL)
,m_bViewLineBuilding(false)
,m_pLineManagerLock(TextProcess::Utils::IReadWriteLock::CreateReadWriteLock())
{
}

CReadBookTPLView2::~CReadBookTPLView2(void)
{
	StopViewLineBuilder();

	delete m_pLineManagerLock;
}

void CReadBookTPLView2::OnDraw(wxDC *pDC)
{
	CReadBookTPLDoc2* pDoc = GetReadBookDoc();

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

		pViewLine = GetNextLine(pViewLine);

		if (pViewLine == NULL)
			break;

		pViewLine->AccessLine();
	}//for each line

	pDC->SetFont(pOldFont);
	pDC->SetTextForeground(oldColor);
	pDC->SetBackground(oldBkColor);
}

void CReadBookTPLView2::CalculateViewSize()
{
	CReadBookTPLDoc2 * pDoc = GetReadBookDoc();

	m_nViewSize = pDoc->GetBufferSize();
}

void CReadBookTPLView2::Recalculate()
{
	CReadBookView::Recalculate();

	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);
	StopViewLineBuilder();
	StartViewLineBuilder();
}

wxInt32 CReadBookTPLView2::ScrollLine(wxInt16 nDelta)
{
	if (!GetReadBookDoc()->IsDocumentLoading() || !m_bViewLineBuilding)
		return GetReadBookDoc()->GetCurrentLine();

	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);
	if (nDelta < 0)
	{
		for(int i = 0; i > nDelta; i--)
		{
			TextProcess::View::IViewLine * pViewLine =  GetPreviousLine(m_pViewLine);

			if (pViewLine == NULL) 
				break;

			pViewLine->AccessLine();

			m_pViewLine = pViewLine;
		}
	}
	else if (nDelta > 0)
	{
		for(int i = 0; i < nDelta; i++)
		{
			TextProcess::View::IViewLine * pViewLine = GetNextLine(m_pViewLine);
			
			if (pViewLine == NULL)
				break;
			pViewLine->AccessLine();

			m_pViewLine = pViewLine;
		}
	}

	wxFileOffset offset = m_pViewLine->GetDocumentLine()->GetOffset();
	GetReadBookDoc()->SetCurrentLine(offset + m_pViewLine->GetOffset());

	return GetReadBookDoc()->GetCurrentLine();
}

wxInt32 CReadBookTPLView2::ScrollToLine(wxInt32 nLine)
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
			usleep(100);
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

	wxFileOffset offset = m_pViewLine->GetDocumentLine()->GetOffset();
	GetReadBookDoc()->SetCurrentLine(offset + m_pViewLine->GetOffset());

	return GetReadBookDoc()->GetCurrentLine();
}

void CReadBookTPLView2::StartViewLineBuilder()
{
	if (!GetReadBookDoc()->IsDocumentLoading())
		return;

	wxFileOffset docPos = GetReadBookDoc()->GetCurrentLine();

	m_pViewLineManager.reset(TextProcess::View::CViewObjectFactory::CreateLineManager());
	m_pViewLine = NULL;

	m_pViewLineBuilderPrev.reset(TextProcess::View::CViewObjectFactory::CreateLineBuilder());

	CReadBookDC dc(GetCanvas());
	CViewBuilderGraphic2 graphic(&dc);

	m_pClientRect.reset(new wxRect(GetClientRect()));

	m_pViewLineBuilderPrev->SetBuilderDirection(TextProcess::Prev);
	m_pViewLineBuilderPrev->SetClientArea(m_pClientRect.get());
	m_pViewLineBuilderPrev->SetDocumentLineManager(GetReadBookDoc()->GetDocumentLineManager());
	m_pViewLineBuilderPrev->SetDocumentLineOffset(docPos);
	m_pViewLineBuilderPrev->SetGraphics(&graphic);
	m_pViewLineBuilderPrev->SetViewFont(wxGetApp().GetPreference()->GetFont());
	m_pViewLineBuilderPrev->SetViewLineManager(m_pViewLineManager.get());
	m_pViewLineBuilderPrev->SetViewLineOffset(0);
	m_pViewLineBuilderPrev->SetBuildLineCount(m_nPageSize * 3);
	m_pViewLineBuilderPrev->SetWaitForLineAccessed(0);
	m_pViewLineBuilderPrev->BuildLines();

	m_pViewLineBuilderNext.reset(TextProcess::View::CViewObjectFactory::CreateLineBuilder());

	m_pViewLineBuilderNext->SetBuilderDirection(TextProcess::Next);
	m_pViewLineBuilderNext->SetClientArea(m_pClientRect.get());
	m_pViewLineBuilderNext->SetDocumentLineManager(GetReadBookDoc()->GetDocumentLineManager());
	m_pViewLineBuilderNext->SetDocumentLineOffset(docPos);
	m_pViewLineBuilderNext->SetGraphics(&graphic);
	m_pViewLineBuilderNext->SetViewFont(wxGetApp().GetPreference()->GetFont());
	m_pViewLineBuilderNext->SetViewLineManager(m_pViewLineManager.get());
	m_pViewLineBuilderNext->SetViewLineOffset(0);
	m_pViewLineBuilderNext->SetBuildLineCount(m_nPageSize * 3);
	m_pViewLineBuilderPrev->SetWaitForLineAccessed(0);
	m_pViewLineBuilderNext->SetWaitForLineAccessed(0);
	m_pViewLineBuilderNext->BuildLines();

	m_bViewLineBuilding = true;
}

void CReadBookTPLView2::StopViewLineBuilder()
{
	m_bViewLineBuilding = false;
}

TextProcess::View::IViewLine * CReadBookTPLView2::GetPreviousLine(TextProcess::View::IViewLine * pCurViewLine)
{
	TextProcess::View::IViewLine * pViewLine = 
		m_pViewLineManager->GetPrevLine(pCurViewLine, 0);

	if (pViewLine == NULL && m_pViewLineManager->IsHasAllPreviousLines()) 
		return NULL;
	else if (pViewLine == NULL)
	{
		wxFileOffset doc_offset = pCurViewLine->GetDocumentLine()->GetOffset();
		wxFileOffset view_offset = pCurViewLine->GetOffset();
		GetReadBookDoc()->SetCurrentLine(doc_offset + view_offset);

		StopViewLineBuilder();
		StartViewLineBuilder();

		std::auto_ptr<TextProcess::View::IViewLineMatcher> pMatcher(TextProcess::View::CViewObjectFactory::CreateLineMatcher(doc_offset, view_offset));
		pViewLine = m_pViewLineManager->FindLine(pMatcher.get());
		pViewLine = m_pViewLineManager->GetPrevLine(pViewLine);
	}

	return pViewLine;
}

TextProcess::View::IViewLine * CReadBookTPLView2::GetNextLine(TextProcess::View::IViewLine * pCurViewLine)
{
	TextProcess::View::IViewLine * pViewLine = 
		m_pViewLineManager->GetNextLine(pCurViewLine, 0);

	if (pViewLine == NULL && m_pViewLineManager->IsHasAllNextLines())
		 return NULL;
	else if (pViewLine == NULL)
	{
		wxFileOffset doc_offset = pCurViewLine->GetDocumentLine()->GetOffset();
		wxFileOffset view_offset = pCurViewLine->GetOffset();
		GetReadBookDoc()->SetCurrentLine(doc_offset + view_offset);

		wxFileOffset saved_doc_offset = m_pViewLine->GetDocumentLine()->GetOffset();
		wxFileOffset saved_view_offset = m_pViewLine->GetOffset();
	
		StopViewLineBuilder();
		StartViewLineBuilder();

		std::auto_ptr<TextProcess::View::IViewLineMatcher> pMatcher(TextProcess::View::CViewObjectFactory::CreateLineMatcher(doc_offset, view_offset));
		pViewLine = m_pViewLineManager->FindLine(pMatcher.get());
		pViewLine = m_pViewLineManager->GetNextLine(pViewLine);

		std::auto_ptr<TextProcess::View::IViewLineMatcher> pMatcher2(TextProcess::View::CViewObjectFactory::CreateLineMatcher(saved_doc_offset, saved_view_offset));
		m_pViewLine = m_pViewLineManager->FindLine(pMatcher2.get());
	}

	return pViewLine;

}

