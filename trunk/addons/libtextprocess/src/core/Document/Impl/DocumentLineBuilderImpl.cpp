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
	InitBuffers();

    const wxByte * pFileBegin = GetDocumentFile()->GetBuffer();
    const wxByte * pFileEnd = pFileBegin + GetDocumentFile()->GetLength();
    const wxByte * pStartPos = pFileBegin + GetDocumentOffset();
    const wxByte * pEndPos = 0;
    const wxByte * pCR = reinterpret_cast<const wxByte *>(m_CRBuffer.data());
    const wxByte * pLF = reinterpret_cast<const wxByte *>(m_LFBuffer.data());
	const wxByte * pCREnd = pCR + m_CRLength;
	const wxByte * pLFEnd = pLF + m_LFLength;
    const wxByte * pchEOL = NULL;

    TextProcess::Document::IDocumentLine * pLastLine =
		GetDocumentLineManager()->FindLine(CDocumentObjectFactory::CreateLineMatcher(GetDocumentOffset()), 0);

	if (pLastLine != NULL)
	{
		pStartPos = pFileBegin + pLastLine->GetOffset();
	}
	else
	{
        pchEOL = std::find_end(pFileBegin, pStartPos,
            pCR, pCREnd);

		if (pStartPos != pchEOL)
		{
			pStartPos = pchEOL + m_CRLength;
		}
		else
		{
			pStartPos = pFileBegin;
		}
	}
	
    if (GetBuilderDirection() == TextProcess::Next)
    {
        pEndPos = pFileEnd;
    }
    else
    {
        pEndPos = pFileBegin;

        std::swap(pEndPos, pStartPos);
    }

    while(!m_Cancel && pStartPos < pEndPos)
    {
        int length = 0;
        int offset = 0;

        if (GetBuilderDirection() == TextProcess::Next)
        {
            pchEOL = std::search(pStartPos, pEndPos, pCR, pCREnd);
            length = pchEOL - pStartPos;
            offset = pStartPos - pFileBegin;

            if (length > m_LFLength)
            {
                if (!memcmp(pStartPos + length - m_LFLength, pLF, m_LFLength))
                {
                    length -= m_LFLength;
                }
            }

            pStartPos = pchEOL + m_CRLength;
        }
        else
        {
            pchEOL = std::find_end(pStartPos, pEndPos,
                pCR, pCREnd);

			if (pchEOL == pEndPos)
			{
				offset = 0;
				length = pEndPos - pFileBegin + 1;
			}
			else
			{
				offset = (pchEOL + m_CRLength) - pFileBegin;
				length = pEndPos - pchEOL - m_CRLength + 1;
			}

            pEndPos = pchEOL - 1;

            if (pEndPos - m_LFLength + 1 > pStartPos)
            {
                if (!memcmp(pEndPos - m_LFLength + 1, pLF, m_LFLength))
                    pEndPos -= m_LFLength;
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

	const wxByte * pSpace = reinterpret_cast<const wxByte *>(m_SpaceBuffer.data());
    const wxByte * pTab = reinterpret_cast<const wxByte *>(m_TabBuffer.data());

    while(pBegin < pEnd)
    {
        if (!memcmp(pBegin, pSpace, m_SpaceLength))
		{
	        pBegin += m_SpaceLength;
		}
		else if (!memcmp(pBegin, pTab, m_TabLength))
		{
            pBegin += m_TabLength;
		}
		else
		{
			return 0;
		}
    }

    return 1;
}

void TextProcess::Document::Impl::CDocumentLineBuilderImpl::InitBuffers()
{
	wxMBConv * pEncoding = GetDocumentFile()->GetEncoding();
	m_SpaceBuffer = pEncoding->cWC2MB(wxT(" "));
	m_TabBuffer = pEncoding->cWC2MB(wxT("\t"));
	m_SpaceLength = strlen(m_SpaceBuffer.data());
	m_TabLength = strlen(m_TabBuffer.data());
	m_CRBuffer = pEncoding->cWC2MB(wxT("\n"));
	m_LFBuffer = pEncoding->cWC2MB(wxT("\r"));
	m_CRLength = strlen(m_CRBuffer.data());
	m_LFLength = strlen(m_LFBuffer.data());
}