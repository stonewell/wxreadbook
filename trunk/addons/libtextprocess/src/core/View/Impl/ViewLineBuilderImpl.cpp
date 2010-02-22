#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#ifdef HAVE_MEMORY
#include <memory>
#endif
#else
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
	std::auto_ptr<TextProcess::Document::IDocumentLineMatcher> pMatcher(TextProcess::Document::CDocumentObjectFactory::CreateLineMatcher(GetDocumentLineOffset()));
	TextProcess::Document::IDocumentLine * pDocLine =
		GetDocumentLineManager()->FindLine(pMatcher.get());

	if (pDocLine == NULL)
	{
		return 1;
	}

	float width = GetViewFont()->GetPointSize();
	wxFileOffset viewLineOffset = GetViewLineOffset();

	int defaultLineCharSize = GetClientArea()->GetWidth() / width;

	if (GetBuilderDirection() == TextProcess::Prev)
	{
		if (viewLineOffset > 0)
		{
		}

		pDocLine = GetDocumentLineManager()->GetPrevLine(pDocLine);
	}

	IViewLine * pNextLine = NULL;
	IViewLine * pFirstDocViewLine = NULL;

	wxUint32 nBuildLineCount = GetBuildLineCount();

	while (!m_Cancel && pDocLine != NULL)
	{
		pFirstDocViewLine = NULL;
		pNextLine = NULL;
		wxChar * pBuf = NULL;
		wxFileOffset pBufLen = 0;
		long cur_all_line_width = 0;
		bool use_readonly_string = true;

		pDocLine->GetData(0, pDocLine->GetDecodedLength(), &pBuf, &pBufLen);

		if (pDocLine->GetDecodedLength() > 200)
			use_readonly_string = false;

		TextProcess::Utils::Impl::wxReadOnlyString docLineData(pBuf, pBufLen);

		if (use_readonly_string && viewLineOffset > 0)
		{
			docLineData.ReadOnlyResize(viewLineOffset);

			wxCoord width, height;

			GetGraphics()->GetTextExtent(docLineData, &width, &height, GetViewFont());

			cur_all_line_width = width;
		}

		while (!m_Cancel && viewLineOffset < pDocLine->GetDecodedLength())
		{

			wxFileOffset viewLineSize = defaultLineCharSize;

			if (viewLineSize + viewLineOffset > pDocLine->GetDecodedLength())
			{
				viewLineSize = pDocLine->GetDecodedLength() - viewLineOffset;
			}

			if (use_readonly_string)
			{
				FixViewLineSize(&docLineData,
					viewLineOffset, viewLineSize,
					cur_all_line_width);
			}
			else
			{
				FixViewLineSize(docLineData,
					viewLineOffset, viewLineSize);
			}

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

            if (nBuildLineCount > 0)
                nBuildLineCount--;

            if (!nBuildLineCount)
            {
				if (!GetWaitForLineAccessed()) 
					return 2;

				while (!m_Cancel)
				{
					if (pLine->WaitForAccessing(500) != WAIT_TIMEOUT)
					{
						break;
					}
				}

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

		if (pDocLine != NULL)
		{
			pDocLine->AccessLine();

			std::auto_ptr<IViewLineMatcher> pMatcher(CViewObjectFactory::CreateLineMatcher(pDocLine->GetOffset(), 0));
			if (GetViewLineManager()->FindLine(pMatcher.get(), false) != NULL)
			{

				break;
			}
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

	if (size <= 0)
		size = 1;
}

void TextProcess::View::Impl::CViewLineBuilderImpl::FixViewLineSize(const wxString & docLineData,
    wxFileOffset offset, wxFileOffset & size)
{
	wxCoord width = 0;
	wxCoord height = 0;

	wxString data = docLineData.substr(offset, size);

	GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());

    do
    {
        if (width < GetClientArea()->GetWidth())
        {
			if ((size_t)(offset + size + 1) <= docLineData.length())
			{
				data = docLineData.substr(offset, size + 1);
				GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());

				while (!m_Cancel &&
					width < GetClientArea()->GetWidth())
				{

					size++;
					if ((size_t)(offset + size + 1) > docLineData.length())
						break;

					data = docLineData.substr(offset, size + 1);
					GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());
				}
			}
        }
        else if (width > GetClientArea()->GetWidth())
        {
            size--;

			if (size > 0)
			{
				data = docLineData.substr(offset, size);
				GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());

				while (!m_Cancel &&
					width > GetClientArea()->GetWidth() &&
					size > 0)
				{
					size--;

					if (size <= 0)
						break;

					data = docLineData.substr(offset, size);
					GetGraphics()->GetTextExtent(data, &width, &height, GetViewFont());
				}
			}
        }//
    }
    while(false);

	if (size <= 0)
		size = 1;
}

