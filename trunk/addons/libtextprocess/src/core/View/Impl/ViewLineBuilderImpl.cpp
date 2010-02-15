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
TPL_PRINTF("View Line Builder %d Started\n", GetBuilderDirection());
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
	wxCoord width = 0;
	wxCoord height = 0;

    wxFileOffset newSize = pDocLineData->ReadOnlyResize(offset + size);

	GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());

    long last_width = width;

    do
    {
		if (width < curAllViewLineWidth)
			break;

        if (width - curAllViewLineWidth < GetClientArea()->GetWidth())
        {
            newSize = pDocLineData->ReadOnlyResize(offset + size + 1);

            if (newSize != offset + size + 1)
            {
                break;
            }

            last_width = width;
            GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());

            while (!m_Cancel &&
                (width - curAllViewLineWidth) < GetClientArea()->GetWidth() &&
                (size_t)(offset + size) <= pDocLineData->GetPchDataLength())
            {
                size++;
                newSize = pDocLineData->ReadOnlyResize(offset + size + 1);

                if (newSize != offset + size + 1)
                    break;

                last_width = width;
                GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());
            }
        }
        else if (width - curAllViewLineWidth > GetClientArea()->GetWidth())
        {
            size--;
            newSize = pDocLineData->ReadOnlyResize(offset + size);

            if (newSize != offset + size)
                break;

            last_width = width;
            GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());

            while (!m_Cancel &&
                (width - curAllViewLineWidth) > GetClientArea()->GetWidth() &&
                size > 0)
            {
                size--;
                newSize = pDocLineData->ReadOnlyResize(offset + size);

                if (newSize != offset + size)
                    break;

                last_width = width;
                GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, GetViewFont());
            }
        }//
    }
    while(false);

	curAllViewLineWidth = last_width;
}

