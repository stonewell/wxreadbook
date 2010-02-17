#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_MEMORY
#include <memory>
#endif

TextProcess::View::Impl::CViewLineBuilderImpl::CViewLineBuilderImpl(void) :
INIT_PROPERTY(ClientArea, NULL)
,INIT_PROPERTY(ViewFont, NULL)
,INIT_PROPERTY(DocumentLineOffset, 0)
,INIT_PROPERTY(ViewLineOffset, 0)
,INIT_PROPERTY(DocumentLineManager, NULL)
,INIT_PROPERTY(ViewLineManager, NULL)
,INIT_PROPERTY(Graphics, NULL)
,INIT_PROPERTY(Cancel, 0)
{
}

TextProcess::View::Impl::CViewLineBuilderImpl::~CViewLineBuilderImpl(void)
{
}

int TextProcess::View::Impl::CViewLineBuilderImpl::BuildLines()
{
TPL_PRINTF("View Line Builder %d Started, %ld\n", GetBuilderDirection(), GetDocumentLineOffset());
	std::auto_ptr<TextProcess::Document::IDocumentLineMatcher> pMatcher(TextProcess::Document::CDocumentObjectFactory::CreateLineMatcher(GetDocumentLineOffset()));
	TextProcess::Document::IDocumentLine * pDocLine =
		GetDocumentLineManager()->FindLine(pMatcher.get());

	if (pDocLine == NULL)
	{
TPL_PRINTF("View Line Builder %d Stopped 1\n", GetBuilderDirection());
		return 1;
	}

	if (GetBuilderDirection() == TextProcess::Prev)
		pDocLine = GetDocumentLineManager()->GetPrevLine(pDocLine);

	float width = GetViewFont()->GetPointSize();
	wxFileOffset viewLineOffset = GetViewLineOffset();

	int defaultLineCharSize = GetClientArea()->GetWidth() / width;

	IViewLine * pNextLine = NULL;
	IViewLine * pFirstDocViewLine = NULL;

	wxUint32 nBuildLineCount = GetBuildLineCount();

    if (!nBuildLineCount)
    {
        TPL_PRINTF("ViewLineBuilder %d BuildLineCount is zero\n", GetBuilderDirection());
    }

	while (!m_Cancel && pDocLine != NULL)
	{
TPL_PRINTF("View Line Builder %d Running 1\n", GetBuilderDirection());
		pFirstDocViewLine = NULL;
		pNextLine = NULL;
		wxChar * pBuf = NULL;
		wxFileOffset pBufLen = 0;
		long cur_all_line_width = 0;

		pDocLine->GetData(0, pDocLine->GetDecodedLength(), &pBuf, &pBufLen);

TPL_PRINTF("View Line Builder %d Running 2\n", GetBuilderDirection());
		TextProcess::Utils::Impl::wxReadOnlyString docLineData(pBuf, pBufLen);

		while (!m_Cancel && viewLineOffset < pDocLine->GetDecodedLength())
		{
TPL_PRINTF("View Line Builder %d Running 3\n", GetBuilderDirection());
			wxFileOffset viewLineSize = defaultLineCharSize;

			if (viewLineSize + viewLineOffset > pDocLine->GetDecodedLength())
			{
				viewLineSize = pDocLine->GetDecodedLength() - viewLineOffset;
			}

TPL_PRINTF("View Line Builder %d Running 4\n", GetBuilderDirection());
			FixViewLineSize(&docLineData,
                viewLineOffset, viewLineSize,
                cur_all_line_width);
TPL_PRINTF("View Line Builder %d Running 5\n", GetBuilderDirection());

			if (m_Cancel) break;

			IViewLine * pLine =
                CViewObjectFactory::CreateViewLine(viewLineOffset, viewLineSize, pDocLine);

TPL_PRINTF("View Line Builder %d Running 6\n", GetBuilderDirection());
			if (pFirstDocViewLine == NULL && GetBuilderDirection() == TextProcess::Prev)
			{
TPL_PRINTF("View Line Builder %d Running 7\n", GetBuilderDirection());
				GetViewLineManager()->AddPrevLine(pLine, pFirstDocViewLine);
				pFirstDocViewLine = pLine;
				pNextLine = pLine;
TPL_PRINTF("View Line Builder %d Running 8\n", GetBuilderDirection());
			}
			else
			{
TPL_PRINTF("View Line Builder %d Running 9\n", GetBuilderDirection());
				GetViewLineManager()->AddNextLine(pLine, pNextLine);
				pNextLine = pLine;
TPL_PRINTF("View Line Builder %d Running 10\n", GetBuilderDirection());
			}

TPL_PRINTF("New ViewLine Added %d\n", GetBuilderDirection());

            if (nBuildLineCount > 0)
                nBuildLineCount--;

            if (!nBuildLineCount)
            {
TPL_PRINTF("ViewLineBuilder %d Wait for last line access offset=%ld, %ld\n", GetBuilderDirection(), pDocLine->GetOffset(), pLine->GetOffset());
				while (!m_Cancel)
				{
					if (pLine->WaitForAccessing(500) != WAIT_TIMEOUT)
					{
TPL_PRINTF("ViewLineBuilder %d Wait quit, %ld, %ld\n", GetBuilderDirection(), pDocLine->GetOffset(), pLine->GetOffset());
						break;
					}
				}

TPL_PRINTF("ViewLineBuilder %d last line waited offset=%ld, %ld\n", GetBuilderDirection(), pDocLine->GetOffset(), pLine->GetOffset());
                nBuildLineCount = GetBuildLineCount();
            }

			viewLineOffset += viewLineSize;
		}

		if (m_Cancel) break;

		viewLineOffset = 0;

		if (GetBuilderDirection() == TextProcess::Next)
			pDocLine = GetDocumentLineManager()->GetNextLine(pDocLine);
		else
			pDocLine = GetDocumentLineManager()->GetPrevLine(pDocLine);
TPL_PRINTF("View Builder %d Get New Doc Line\n", GetBuilderDirection());
		if (pDocLine != NULL)
		{
			pDocLine->AccessLine();

TPL_PRINTF("View Builder %d Get New Doc Line check\n", GetBuilderDirection());
			std::auto_ptr<IViewLineMatcher> pMatcher(CViewObjectFactory::CreateLineMatcher(pDocLine->GetOffset(), 0));
			if (GetViewLineManager()->FindLine(pMatcher.get(), false) != NULL)
			{
TPL_PRINTF("View Builder %d Get New Doc Line check existing\n", GetBuilderDirection());
				break;
			}
		}
TPL_PRINTF("View Builder %d Get New Doc Line check pass\n", GetBuilderDirection());
	}

	if (GetBuilderDirection() == TextProcess::Next)
		GetViewLineManager()->HasAllNextLines();
	else
		GetViewLineManager()->HasAllPreviousLines();

TPL_PRINTF("View Line Builder %d Stopped 2\n", GetBuilderDirection());
	return true;
}

void TextProcess::View::Impl::CViewLineBuilderImpl::Cancel()
{
	m_Cancel = 1;

	if (GetBuilderDirection() == TextProcess::Next)
		GetViewLineManager()->HasAllNextLines();
	else
		GetViewLineManager()->HasAllPreviousLines();
}

void TextProcess::View::Impl::CViewLineBuilderImpl::FixViewLineSize(TextProcess::Utils::Impl::wxReadOnlyString * pDocLineData,
    wxFileOffset offset, wxFileOffset & size, long & curAllViewLineWidth)
{
TPL_PRINTF("FixViewLine %d Size Start\n", GetBuilderDirection());
	wxCoord width = 0;
	wxCoord height = 0;

    wxFileOffset newSize = pDocLineData->ReadOnlyResize(offset + size);

TPL_PRINTF("FixViewLine %d Size 1\n", GetBuilderDirection());
	GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());

TPL_PRINTF("FixViewLine %d Size 2\n", GetBuilderDirection());
    long last_width = width;

    do
    {
TPL_PRINTF("FixViewLine %d Size 3\n", GetBuilderDirection());
		if (width < curAllViewLineWidth)
			break;

TPL_PRINTF("FixViewLine %d Size 4\n", GetBuilderDirection());
        if (width - curAllViewLineWidth < GetClientArea()->GetWidth())
        {
TPL_PRINTF("FixViewLine %d Size 5\n", GetBuilderDirection());
            newSize = pDocLineData->ReadOnlyResize(offset + size + 1);
TPL_PRINTF("FixViewLine %d Size 6\n", GetBuilderDirection());

            if (newSize != offset + size + 1)
            {
                break;
            }

TPL_PRINTF("FixViewLine %d Size 7\n", GetBuilderDirection());
            last_width = width;
            GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());

TPL_PRINTF("FixViewLine %d Size 8\n", GetBuilderDirection());
            while (!m_Cancel &&
                (width - curAllViewLineWidth) < GetClientArea()->GetWidth() &&
                (size_t)(offset + size) <= pDocLineData->GetPchDataLength())
            {
TPL_PRINTF("FixViewLine %d Size 9\n", GetBuilderDirection());
                size++;
                newSize = pDocLineData->ReadOnlyResize(offset + size + 1);

TPL_PRINTF("FixViewLine %d Size 10\n", GetBuilderDirection());
                if (newSize != offset + size + 1)
                    break;

TPL_PRINTF("FixViewLine %d Size 11\n", GetBuilderDirection());
                last_width = width;
                GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());
TPL_PRINTF("FixViewLine %d Size 12\n", GetBuilderDirection());
            }
        }
        else if (width - curAllViewLineWidth > GetClientArea()->GetWidth())
        {
TPL_PRINTF("FixViewLine %d Size 13\n", GetBuilderDirection());
            size--;
            newSize = pDocLineData->ReadOnlyResize(offset + size);

TPL_PRINTF("FixViewLine %d Size 14\n", GetBuilderDirection());
            if (newSize != offset + size)
                break;

TPL_PRINTF("FixViewLine %d Size 15\n", GetBuilderDirection());
            last_width = width;
            GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());

TPL_PRINTF("FixViewLine %d Size 16\n", GetBuilderDirection());
            while (!m_Cancel &&
                (width - curAllViewLineWidth) > GetClientArea()->GetWidth() &&
                size > 0)
            {
TPL_PRINTF("FixViewLine %d Size 17\n", GetBuilderDirection());
                size--;
                newSize = pDocLineData->ReadOnlyResize(offset + size);

TPL_PRINTF("FixViewLine %d Size 18\n", GetBuilderDirection());
                if (newSize != offset + size)
                    break;

TPL_PRINTF("FixViewLine %d Size 19\n", GetBuilderDirection());
                last_width = width;
                GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());
TPL_PRINTF("FixViewLine %d Size 20\n");
            }
        }//
    }
    while(false);

	curAllViewLineWidth = last_width;
TPL_PRINTF("FixViewLine %d Size Quit\n", GetBuilderDirection());
}

