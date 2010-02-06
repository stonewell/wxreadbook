#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

#include <algorithm>

TextProcess::Document::Impl::CDocumentLineBuilderImpl::CDocumentLineBuilderImpl(void) :
INIT_PROPERTY(DocumentOffset, 0),
INIT_PROPERTY(DocumentLineManager, NULL),
INIT_PROPERTY(DocumentFile, NULL),
INIT_PROPERTY(Cancel, 0)
{
}

TextProcess::Document::Impl::CDocumentLineBuilderImpl::~CDocumentLineBuilderImpl(void)
{
}

int TextProcess::Document::Impl::CDocumentLineBuilderImpl::BuildLines()
{
    const wxByte * pFileBegin = GetDocumentFile()->GetBuffer();
    const wxByte * pFileEnd = pFileBegin + GetDocumentFile()->GetLength();
    const wxByte * pStartPos = pFileBegin + GetDocumentOffset();
    const wxByte * pEndPos = 0;
    const wxByte pLineEnd[] = {'\n'};
    TextProcess::Document::IDocumentLine * pLastLine =
		GetDocumentLineManager()->FindLine(CDocumentObjectFactory::CreateLineMatcher(GetDocumentOffset()), 0);

    if (GetBuilderDirection() == TextProcess::Next)
    {
        pEndPos = pFileEnd;
    }
    else
    {
        pEndPos = pFileBegin - 1;

        std::swap(pEndPos, pStartPos);
    }

    while(!m_Cancel && pStartPos < pEndPos)
    {
        const wxByte * pchEOL = NULL;
        int length = 0;
        int offset = 0;

        if (GetBuilderDirection() == TextProcess::Next)
        {
            pchEOL = std::find(pStartPos, pEndPos, '\n');
            length = pchEOL - pStartPos;
            offset = pStartPos - pFileBegin;

            if (length > 0)
            {
                if (*(pStartPos + length - 1) == '\r')
                {
                    length --;
                }
            }

            pStartPos = pchEOL + 1;
        }
        else
        {
            pchEOL = std::find_end(pStartPos, pEndPos,
                pLineEnd, pLineEnd + 1);

            offset = (pchEOL + 1) - pFileBegin;
            length = pEndPos - pchEOL;

            pEndPos = pchEOL - 1;

            if (pEndPos > pStartPos)
            {
                if (*pEndPos == '\r')
                    pEndPos--;
            }
        }

        if (!m_Cancel && !IsEmptyLine(offset, length))
        {
            TextProcess::Document::IDocumentLine * pDocLine =
                CDocumentObjectFactory::CreateDocumentLine(offset, length, GetDocumentFile());

            if (GetBuilderDirection() == TextProcess::Next)
                GetDocumentLineManager()->AddNextLine(pDocLine, pLastLine);
            else
                GetDocumentLineManager()->AddPrevLine(pDocLine, pLastLine);

            pLastLine = pDocLine;
        }
    }

	return 1;
}

void TextProcess::Document::Impl::CDocumentLineBuilderImpl::Cancel()
{
	m_Cancel = 1;
	GetDocumentLineManager()->HasAllLines();
}

int TextProcess::Document::Impl::CDocumentLineBuilderImpl::IsEmptyLine(int offset, int length)
{
    const wxByte * pBegin = GetDocumentFile()->GetBuffer() + offset;
    const wxByte * pEnd = pBegin + length;

    while(pBegin < pEnd)
    {
        if ((*pBegin) != ' '
            && (*pBegin) != '\t')
            return 0;

        pBegin++;
    }

    return 1;
}

