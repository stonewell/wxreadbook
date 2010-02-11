#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

#include <memory>

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
	std::auto_ptr<TextProcess::Document::IDocumentLineMatcher> pMatcher(TextProcess::Document::CDocumentObjectFactory::CreateLineMatcher(GetDocumentLineOffset()));
	TextProcess::Document::IDocumentLine * pDocLine =
		GetDocumentLineManager()->FindLine(pMatcher.get());

	if (pDocLine == NULL)
		return 1;

	if (GetBuilderDirection() == TextProcess::Prev)
		pDocLine = GetDocumentLineManager()->GetPrevLine(pDocLine);

	float width = GetViewFont()->GetPointSize();
	wxFileOffset viewLineOffset = GetViewLineOffset();

	int defaultLineCharSize = GetClientArea()->GetWidth() / width;

	IViewLine * pNextLine = NULL;
	IViewLine * pFirstDocViewLine = NULL;

	while (!m_Cancel && pDocLine != NULL)
	{
		pFirstDocViewLine = NULL;
		pNextLine = NULL;
		wxChar * pBuf = NULL;
		wxFileOffset pBufLen = 0;
		long cur_all_line_width = 0;

		pDocLine->GetData(0, pDocLine->GetLength(), &pBuf, &pBufLen);

		TextProcess::Utils::wxReadOnlyString docLineData(pBuf, pBufLen);

		while (!m_Cancel && viewLineOffset < pDocLine->GetLength())
		{
			wxFileOffset viewLineSize = defaultLineCharSize;

			if (viewLineSize + viewLineOffset > pDocLine->GetLength())
			{
				viewLineSize = pDocLine->GetLength() - viewLineOffset;
			}

			FixViewLineSize(&docLineData,
                viewLineOffset, viewLineSize,
                cur_all_line_width);

			if (m_Cancel) break;

			IViewLine * pLine =
                CViewObjectFactory::CreateViewLine(viewLineOffset, viewLineSize, pDocLine);

			if (pFirstDocViewLine == NULL && GetBuilderDirection() == TextProcess::Prev)
			{
				GetViewLineManager()->AddPrevLine(pLine, pFirstDocViewLine);
				pFirstDocViewLine = pLine;
				pNextLine = pLine;
			}
			else
			{
				GetViewLineManager()->AddNextLine(pLine, pNextLine);
				pNextLine = pLine;
			}

			viewLineOffset += viewLineSize;
		}

		if (m_Cancel) break;

		viewLineOffset = 0;

		if (GetBuilderDirection() == TextProcess::Next)
			pDocLine = GetDocumentLineManager()->GetNextLine(pDocLine);
		else
			pDocLine = GetDocumentLineManager()->GetPrevLine(pDocLine);

		if (pDocLine != NULL &&
			GetViewLineManager()->FindLine(CViewObjectFactory::CreateLineMatcher(pDocLine->GetOffset(), 0), false) != NULL)
		{
			break;
		}
	}

	if (GetBuilderDirection() == TextProcess::Next)
		GetViewLineManager()->HasAllNextLines();
	else
		GetViewLineManager()->HasAllPreviousLines();

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

void TextProcess::View::Impl::CViewLineBuilderImpl::FixViewLineSize(TextProcess::Utils::wxReadOnlyString * pDocLineData,
    wxFileOffset offset, wxFileOffset & size, long & curAllViewLineWidth)
{
	long width = 0;
	long height = 0;

    wxFileOffset newSize = pDocLineData->ReadOnlyResize(offset + size);

	GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, 0, 0, GetViewFont());

    long last_width = width;

    do
    {
        if (width - curAllViewLineWidth < GetClientArea()->GetWidth())
        {
            newSize = pDocLineData->ReadOnlyResize(offset + size + 1);

            if (newSize != offset + size + 1)
            {
                break;
            }

            last_width = width;
            GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, 0, 0, GetViewFont());

            while (!m_Cancel &&
                (width - curAllViewLineWidth) < GetClientArea()->GetWidth() &&
                (size_t)(offset + size) <= pDocLineData->GetPchDataLength())
            {
                size++;
                newSize = pDocLineData->ReadOnlyResize(offset + size + 1);

                if (newSize != offset + size + 1)
                    break;

                last_width = width;
                GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, 0, 0, GetViewFont());
            }
        }
        else if (width > GetClientArea()->GetWidth())
        {
            size--;
            newSize = pDocLineData->ReadOnlyResize(offset + size);

            if (newSize != offset + size)
                break;

            last_width = width;
            GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, 0, 0, GetViewFont());

            while (!m_Cancel &&
                (width - curAllViewLineWidth) > GetClientArea()->GetWidth() &&
                size > 0)
            {
                size--;
                newSize = pDocLineData->ReadOnlyResize(offset + size);

                if (newSize != offset + size)
                    break;

                last_width = width;
                GetGraphics()->GetTextExtent(*pDocLineData, &width, &height, 0, 0, GetViewFont());
            }
        }//if
    }
    while(false);

	curAllViewLineWidth = last_width;
}
