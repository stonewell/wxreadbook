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

#if wxUSE_UNICODE
#include "../../ns/nsDetector.h"
#include "../../unicode/GBBig5Table.h"
#endif

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
		std::shared_ptr<TextProcess::View::IViewLineMatcher> pMatcher(TextProcess::View::CViewObjectFactory::CreateLineMatcher(currentLine, 0));

		m_pViewLine = 
                m_pViewLineManager->FindLine(pMatcher.get(), 0);

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
                m_pViewLineManager->FindLine(pMatcher.get(), 0);

		if (m_pViewLine == NULL)
		{
			StopViewLineBuilder();
			StartViewLineBuilder(pMatcher->GetDocumentLineOffset(), 
                                 pMatcher->GetViewLineOffset());

			m_pViewLine = 
                    m_pViewLineManager->FindLine(pMatcher.get(), 0);
		}

#ifdef _DEBUG
		printf("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n");
#endif
	}

	if (m_pViewLine == NULL)
		return;

	static int offset = 0;
	static int doc_offset = 0;


#ifdef _DEBUG
	if (m_pViewLine)
	{
		if (offset == m_pViewLine->GetOffset() &&
			doc_offset == m_pViewLine->GetDocumentLine()->GetOffset()) {
            fprintf(stderr, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
            printf("hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh:%p, %d %d %d %d\n", m_pViewLine,
                   (wxInt32)m_pViewLine->GetOffset(), (wxInt32)m_pViewLine->GetDocumentLine()->GetOffset(), 
                   (wxInt32)(m_pViewLine->GetOffset() + m_pViewLine->GetDocumentLine()->GetOffset()),
                   GetReadBookDoc() == NULL ? 0 : GetReadBookDoc()->GetCurrentLine());
		}
		offset = m_pViewLine->GetOffset();
		doc_offset = m_pViewLine->GetDocumentLine()->GetOffset();
	}
#endif

	TextProcess::View::IViewLine * pViewLine = m_pViewLine;

#ifdef _DEBUG
	wxFileOffset * offsets = new wxFileOffset[m_nPageSize * 2];

	for(int i=0;i<m_nPageSize; i++)
	{
		offsets[i * 2 + 0] = -1;
		offsets[i * 2 + 1] = -1;
	}
#endif

	for (int i=0; i < m_nPageSize; i++)
	{
		
#ifdef _DEBUG
		for(int ii = 0; ii < m_nPageSize; ii++)
		{
			if (offsets[ii * 2 + 0] == pViewLine->GetDocumentLine()->GetOffset() &&
				offsets[ii * 2 + 1] == pViewLine->GetOffset())
			{
				printf("Show same line:%d %d", offsets[ii * 2 + 0] , offsets[ii * 2 + 1]);
			}
		}

		offsets[i * 2 + 0] = pViewLine->GetDocumentLine()->GetOffset();
		offsets[i * 2 + 1] = pViewLine->GetOffset();
#endif
		wxString line = 
                pViewLine->GetDocumentLine()->GetData(pViewLine->GetOffset(), 
                                                      pViewLine->GetLength());
#ifdef _DEBUG
        printf("%d:%ls,%d\n", i, line.c_str(),m_nPageSize);
#endif		
		if (GetDisplayAs() != wxReadBook::DisplayAsOriginal)
    	{
       		wxChar tmp1[2], tmp2[2];

        	for(int i=0;i<gb2big5TableSize;i+=2)
        	{
            	tmp1[0] = 
                        GetDisplayAs() == wxReadBook::DisplayAsSimplify ? 
						gb2big5[i + 1] : gb2big5[i]; 
				tmp1[1] = 0;
            	
				tmp2[0] = 
                        GetDisplayAs() == wxReadBook::DisplayAsSimplify ? 
						gb2big5[i] : gb2big5[i + 1]; 
				tmp2[1] = 0;

            	line.Replace(tmp1, tmp2);
        	}
    	}

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

#ifdef _DEBUG
	delete[] offsets;
#endif
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
	if (!GetReadBookDoc()->IsDocumentLoading() || !m_bViewLineBuilding || nDelta == 0)
		return GetReadBookDoc()->GetCurrentLine();

#ifdef _DEBUG
	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$Scroll Line Begin:%d %d %d %d %p\n", 
           nDelta, GetReadBookDoc()->GetCurrentLine(), 
           (wxInt32)m_pViewLine->GetOffset(), (wxInt32)(m_pViewLine->GetDocumentLine()->GetOffset()),
           m_pViewLine);
#endif
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
			{
#ifdef _DEBUG
				printf("scroll line break out:%d\n", i);
#endif
				break;
			}
			pViewLine->AccessLine();

			m_pViewLine = pViewLine;
		}
	}

	wxFileOffset offset = m_pViewLine->GetDocumentLine()->GetOffset();
	GetReadBookDoc()->SetCurrentLine(offset + m_pViewLine->GetOffset());

#ifdef _DEBUG
	printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Scroll Line End:%d %d %d %d %p\n", 
           nDelta, GetReadBookDoc()->GetCurrentLine(), 
           (wxInt32)m_pViewLine->GetOffset(), (wxInt32)offset,
           m_pViewLine);
#endif
	return GetReadBookDoc()->GetCurrentLine();
}

wxInt32 CReadBookTPLView2::ScrollToLine(wxInt32 nLine)
{
	if (!GetReadBookDoc()->IsDocumentLoading() || !m_bViewLineBuilding)
		return GetReadBookDoc()->GetCurrentLine();

	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);

	std::shared_ptr<TextProcess::Document::IDocumentLineMatcher> 
            pDocMatcher(TextProcess::Document::CDocumentObjectFactory::CreateLineMatcher(nLine));

	TextProcess::Document::IDocumentLine * pDocLine =
            GetReadBookDoc()->GetDocumentLineManager()->FindLine(pDocMatcher.get(), 0);

	wxInt32 nViewLineOffset = 0;

	if (pDocLine == NULL)
	{
#ifdef _DEBUG
		printf("ScrollToLine &&&&&&&& %d\n", nLine);
#endif
		GetReadBookDoc()->ScrollDocumentTo(nLine);

		pDocLine =
                GetReadBookDoc()->GetDocumentLineManager()->FindLine(pDocMatcher.get());

		if (pDocLine == NULL)
			return GetReadBookDoc()->GetCurrentLine();

		StopViewLineBuilder();
	}

	nViewLineOffset = nLine - pDocLine->GetOffset();

	std::shared_ptr<TextProcess::View::IViewLineMatcher> 
            pMatcher(TextProcess::View::CViewObjectFactory::CreateLineMatcher(pDocLine->GetOffset(), nViewLineOffset));

	TextProcess::View::IViewLine * pViewLine = 
            m_pViewLineManager->FindLine(pMatcher.get(), 0);

	if (pViewLine == NULL)
	{
		StopViewLineBuilder();

		StartViewLineBuilder(pDocLine->GetOffset(), 
                             nViewLineOffset);

		pViewLine = m_pViewLineManager->FindLine(pMatcher.get(), 0);
	}

	if (pViewLine == NULL)
		return GetReadBookDoc()->GetCurrentLine();

	pViewLine->AccessLine();
	m_pViewLine = pViewLine;

	wxFileOffset offset = m_pViewLine->GetDocumentLine()->GetOffset();
	GetReadBookDoc()->SetCurrentLine(offset + m_pViewLine->GetOffset());

	return GetReadBookDoc()->GetCurrentLine();
}

void CReadBookTPLView2::StartViewLineBuilder()
{
	wxFileOffset docPos = GetReadBookDoc()->GetCurrentLine();

	StartViewLineBuilder(docPos, 0);
}

void CReadBookTPLView2::StartViewLineBuilder(wxFileOffset docOffset, wxFileOffset viewOffset)
{
	if (!GetReadBookDoc()->IsDocumentLoading())
		return;

	CReadBookDC dc(GetCanvas());
	CViewBuilderGraphic2 graphic(&dc);

	m_pClientRect.reset(new wxRect(GetClientRect()));

	m_pViewLineBuilderPrev.reset(TextProcess::View::CViewObjectFactory::CreateLineBuilder());
	m_pViewLineBuilderPrev->SetBuilderDirection(TextProcess::Prev);
	m_pViewLineBuilderPrev->SetClientArea(m_pClientRect.get());
	m_pViewLineBuilderPrev->SetDocumentLineManager(GetReadBookDoc()->GetDocumentLineManager());
	m_pViewLineBuilderPrev->SetDocumentLineOffset(docOffset);
	m_pViewLineBuilderPrev->SetGraphics(&graphic);
	m_pViewLineBuilderPrev->SetViewFont(wxGetApp().GetPreference()->GetFont());
	m_pViewLineBuilderPrev->SetViewLineManager(m_pViewLineManager.get());
	m_pViewLineBuilderPrev->SetViewLineOffset(viewOffset);
	m_pViewLineBuilderPrev->SetBuildLineCount(m_nPageSize * 3);
	m_pViewLineBuilderPrev->SetWaitForLineAccessed(0);
	m_pViewLineBuilderPrev->BuildLines();

	TextProcess::View::IViewLine * pLastLine = m_pViewLineManager->GetTailLine(0);

	bool build_next = true;

	if (pLastLine != NULL)
	{
		viewOffset = pLastLine->GetOffset() + pLastLine->GetLength();
		
		if (viewOffset >= pLastLine->GetDocumentLine()->GetDecodedLength())
		{
			docOffset = pLastLine->GetDocumentLine()->GetOffset() +
                        pLastLine->GetDocumentLine()->GetLength();
			viewOffset = 0;

			std::shared_ptr<TextProcess::Document::IDocumentLineMatcher> 
                    pMatcher(TextProcess::Document::CDocumentObjectFactory::CreateLineMatcher(docOffset));

			TextProcess::Document::IDocumentLine * pNextDocLine =
                    GetReadBookDoc()->GetDocumentLineManager()->FindLine(pMatcher.get(), 0);

			if (pNextDocLine == pLastLine->GetDocumentLine())
			{
				build_next = false;
#ifdef _DEBUG
                printf("build_next false\n");
#endif
			}
		}
	}

	if (build_next)
	{
		m_pViewLineBuilderNext.reset(TextProcess::View::CViewObjectFactory::CreateLineBuilder());
		m_pViewLineBuilderNext->SetBuilderDirection(TextProcess::Next);
		m_pViewLineBuilderNext->SetClientArea(m_pClientRect.get());
		m_pViewLineBuilderNext->SetDocumentLineManager(GetReadBookDoc()->GetDocumentLineManager());
		m_pViewLineBuilderNext->SetDocumentLineOffset(docOffset);
		m_pViewLineBuilderNext->SetGraphics(&graphic);
		m_pViewLineBuilderNext->SetViewFont(wxGetApp().GetPreference()->GetFont());
		m_pViewLineBuilderNext->SetViewLineManager(m_pViewLineManager.get());
		m_pViewLineBuilderNext->SetViewLineOffset(viewOffset);
		m_pViewLineBuilderNext->SetBuildLineCount(m_nPageSize * 3);
		m_pViewLineBuilderPrev->SetWaitForLineAccessed(0);
		m_pViewLineBuilderNext->SetWaitForLineAccessed(0);
		m_pViewLineBuilderNext->BuildLines();
	}
	else
	{
		m_pViewLineManager->HasAllNextLines();
	}

	m_bViewLineBuilding = true;
}

void CReadBookTPLView2::StopViewLineBuilder()
{
	m_bViewLineBuilding = false;

	m_pViewLineManager.reset(TextProcess::View::CViewObjectFactory::CreateLineManager());
	m_pViewLine = NULL;
}

TextProcess::View::IViewLine * CReadBookTPLView2::GetPreviousLine(TextProcess::View::IViewLine * pCurViewLine)
{
	TextProcess::View::IViewLine * pViewLine = 
            m_pViewLineManager->GetPrevLine(pCurViewLine, 0);

	if (pViewLine == NULL && m_pViewLineManager->IsHasAllPreviousLines()) 
		return NULL;
	else if (pViewLine == NULL && pCurViewLine == NULL)
		return NULL;
	else if (pViewLine == NULL)
	{
		wxFileOffset doc_offset = pCurViewLine->GetDocumentLine()->GetOffset();
		wxFileOffset view_offset = pCurViewLine->GetOffset();
		//GetReadBookDoc()->SetCurrentLine(doc_offset + view_offset);

		wxFileOffset saved_doc_offset = m_pViewLine->GetDocumentLine()->GetOffset();
		wxFileOffset saved_view_offset = m_pViewLine->GetOffset();

		StopViewLineBuilder();
		StartViewLineBuilder(doc_offset, view_offset);

		std::shared_ptr<TextProcess::View::IViewLineMatcher> 
                pMatcher(TextProcess::View::CViewObjectFactory::CreateLineMatcher(doc_offset, view_offset));

		pViewLine = m_pViewLineManager->FindLine(pMatcher.get(), 0);
		pViewLine = m_pViewLineManager->GetPrevLine(pViewLine);

		std::shared_ptr<TextProcess::View::IViewLineMatcher> pMatcher2(TextProcess::View::CViewObjectFactory::CreateLineMatcher(saved_doc_offset, saved_view_offset));
		m_pViewLine = m_pViewLineManager->FindLine(pMatcher2.get(), 0);
	}

	return pViewLine;
}

TextProcess::View::IViewLine * CReadBookTPLView2::GetNextLine(TextProcess::View::IViewLine * pCurViewLine)
{
	TextProcess::View::IViewLine * pViewLine = 
            m_pViewLineManager->GetNextLine(pCurViewLine, 0);

	if (pViewLine == NULL && m_pViewLineManager->IsHasAllNextLines())
		return NULL;
	else if (pViewLine == NULL && pCurViewLine == NULL)
		return NULL;
	else if (pViewLine == NULL)
	{
		wxFileOffset doc_offset = pCurViewLine->GetDocumentLine()->GetOffset();
		wxFileOffset view_offset = pCurViewLine->GetOffset();
		//GetReadBookDoc()->SetCurrentLine(doc_offset + view_offset);

		wxFileOffset saved_doc_offset = m_pViewLine->GetDocumentLine()->GetOffset();
		wxFileOffset saved_view_offset = m_pViewLine->GetOffset();

		StopViewLineBuilder();
		StartViewLineBuilder(doc_offset, view_offset);

		std::shared_ptr<TextProcess::View::IViewLineMatcher> pMatcher(TextProcess::View::CViewObjectFactory::CreateLineMatcher(doc_offset, view_offset));
		pViewLine = m_pViewLineManager->FindLine(pMatcher.get(), 0);
		pViewLine = m_pViewLineManager->GetNextLine(pViewLine);

		std::shared_ptr<TextProcess::View::IViewLineMatcher> pMatcher2(TextProcess::View::CViewObjectFactory::CreateLineMatcher(saved_doc_offset, saved_view_offset));
		m_pViewLine = m_pViewLineManager->FindLine(pMatcher2.get(), 0);

#ifdef _DEBUG
		if (m_pViewLine == NULL)
			printf("kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk\n");
#endif
	}

	return pViewLine;
}

void CReadBookTPLView2::UpdateScrollPos(void)
{
	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);

	if (m_pViewLine == NULL)
		SetVertScrollPos(GetReadBookDoc()->GetCurrentLine());
	else
	{
		wxFileOffset docOffset = m_pViewLine->GetDocumentLine()->GetOffset();
		wxFileOffset viewOffset = m_pViewLine->GetDocumentLine()->GetLength() *
                                  ((double)m_pViewLine->GetOffset() / (double)m_pViewLine->GetDocumentLine()->GetDecodedLength());

		SetVertScrollPos(docOffset + viewOffset);
	}
}

wxInt32 CReadBookTPLView2::ScrollPosToLine(wxInt32 nPos)
{
	wxInt32 offset, length, decodedLength;

	if (GetReadBookDoc()->GetDocumentLineInfo(nPos, offset, length, decodedLength))
	{
		if (nPos - offset >= length)
		{
			//handle the last line problem
			return offset + decodedLength - 1;
		}

		return offset + decodedLength * (double)(nPos - offset) / (double)length;
	}
	else
	{
		return nPos;
	}
}

wxInt32 CReadBookTPLView2::ScrollLineToPos(wxInt32 nLine)
{
	wxInt32 offset, length, decodedLength;

	if (GetReadBookDoc()->GetDocumentLineInfo(nLine, offset, length, decodedLength))
	{
		return offset + length * (double)(nLine - offset) / (double)decodedLength;
	}
	else
	{
		return nLine;
	}
}

void CReadBookTPLView2::OnScrollWin(wxScrollWinEvent& event)
{
    //	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);

	CReadBookView::OnScrollWin(event);
}

void CReadBookTPLView2::OnKeyDown(wxKeyEvent& event)
{
    //	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);

	CReadBookView::OnKeyDown(event);
}

void CReadBookTPLView2::OnSize(wxSizeEvent& event)
{
    //	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);

	CReadBookView::OnSize(event);
}

void CReadBookTPLView2::OnMouseWheel(wxMouseEvent & event)
{
    //	TextProcess::Utils::CReadWriteLockAccessor a(m_pLineManagerLock, 0);

	CReadBookView::OnMouseWheel(event);
}

