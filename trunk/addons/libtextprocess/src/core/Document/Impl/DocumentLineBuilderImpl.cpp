#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#ifdef HAVE_ALGORITHM
#include <algorithm>
#endif

#ifdef HAVE_MEMORY
#include <memory>
#endif
#else
#include <memory>
#include <algorithm>
#endif


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
    const wxByte * pchEOL2 = NULL;

	if (pStartPos > pFileEnd)
		pStartPos = pFileEnd;

	if (pStartPos < pFileBegin)
		pStartPos = pFileBegin;

	std::auto_ptr<TextProcess::Document::IDocumentLineMatcher> pMatcher(CDocumentObjectFactory::CreateLineMatcher(GetDocumentOffset()));

	TextProcess::Document::IDocumentLine * pLastLine =
		GetDocumentLineManager()->FindLine(pMatcher.get(), 0);

	if (pLastLine != NULL)
	{
		pStartPos = pFileBegin + pLastLine->GetOffset();
	}
	else
	{
		bool cr = true;
        pchEOL = std::find_end(pFileBegin, pStartPos,
            pCR, pCREnd);
        pchEOL2 = std::find_end(pFileBegin, pStartPos,
            pLF, pLFEnd);

		if (pchEOL2 < pchEOL)
		{
			pchEOL = pchEOL2;
			cr = false;
		}

		if (pStartPos != pchEOL)
		{
			if (GetBuilderDirection() == TextProcess::Next)
			{
				if (cr)
					pStartPos = pchEOL + m_CRLength;
				else if (pchEOL + m_LFLength + m_CRLength < pFileEnd &&
						!memcmp(pchEOL + m_LFLength, pCR, m_CRLength))
				{
					pStartPos = pchEOL + m_LFLength + m_CRLength;
				}
				else
				{
					pStartPos = pchEOL + m_LFLength;
				}
			}
			else
			{
                if (cr && !memcmp(pchEOL - m_LFLength, pLF, m_LFLength))
                {
                    pStartPos = pchEOL - m_LFLength;
                }
				else
				{
					pStartPos = pchEOL;
				}
			}
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

TPL_PRINTF("DocumentLineBuilder %d started at %ld\n", GetBuilderDirection(), pStartPos - pFileBegin);

	wxUint32 nBuildLineCount = GetBuildLineCount();

	if (!nBuildLineCount)
	{
		TPL_PRINTF("DocumentLineBuilder %d BuildLineCount is zero\n", GetBuilderDirection());
	}

    while(!m_Cancel && pStartPos < pEndPos)
    {
        wxFileOffset length = 0;
        wxFileOffset offset = 0;

        if (GetBuilderDirection() == TextProcess::Next)
        {
			bool cr = true;
            pchEOL = std::search(pStartPos, pEndPos, pCR, pCREnd);
            pchEOL2 = std::search(pStartPos, pEndPos, pLF, pLFEnd);

			if (pchEOL2 < pchEOL)
			{
				pchEOL = pchEOL2;
				cr = false;
			}

            length = pchEOL - pStartPos;
            offset = pStartPos - pFileBegin;

			if (cr)
			{
            	pStartPos = pchEOL + m_CRLength;
			}
			else
			{
				pStartPos = pchEOL + m_LFLength;
				
				if (pStartPos + m_CRLength <= pEndPos)
				{
					if (!memcmp(pStartPos, pCR, m_CRLength))
						pStartPos += m_CRLength;
				}
			}

            if (cr && length >= m_LFLength)
            {
                if (!memcmp(pFileBegin + offset + length - m_LFLength, pLF, m_LFLength))
                {
                    length -= m_LFLength;
                }
            }
        }
        else
        {
			bool cr = true;
            pchEOL = std::find_end(pStartPos, pEndPos,
                pCR, pCREnd);
            pchEOL2 = std::find_end(pStartPos, pEndPos,
                pLF, pLFEnd);

			if (pchEOL2 < pchEOL)
			{
				cr = false;
				pchEOL = pchEOL2;
			}

			if (pchEOL == pEndPos)
			{
				offset = 0;
				length = pEndPos - pFileBegin + 1;
			}
			else
			{
				if (cr)
				{
					offset = (pchEOL + m_CRLength) - pFileBegin;
					length = (pEndPos - (pchEOL + m_CRLength)) + 1;

					if (pchEOL - m_LFLength >= pStartPos)
					{
						if (!memcmp(pchEOL - m_LFLength, pLF, m_LFLength))
						{
							pchEOL -= m_LFLength;
						}
					}
				}
				else
				{
					offset = (pchEOL + m_LFLength) - pFileBegin;
					length = (pEndPos - (pchEOL + m_LFLength)) + 1;
				}
			}

            pEndPos = pchEOL - 1;
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

			if (nBuildLineCount > 0)
				nBuildLineCount--;

			if (!nBuildLineCount)
			{
				if (!GetWaitForLineAccessed()) 
					return 2;

TPL_PRINTF("DocumentLineBuilder %d Wait for last line access offset=%ld\n", GetBuilderDirection(), pLastLine->GetOffset());
				while(!m_Cancel) 
				{
					if (pLastLine->WaitForAccessing(500) != WAIT_TIMEOUT)
						break;
				}

TPL_PRINTF("DocumentLineBuilder %d last line waited offset=%ld\n", GetBuilderDirection(), pLastLine->GetOffset());
				nBuildLineCount = GetBuildLineCount();
			}
			else
			{
#ifdef _WIN32
				Sleep(1);
#elif HAVE_NANOSLEEP
				struct timespec req;
				req.tv_sec = 0;
				req.tv_nsec = 1000 * 1000;
				nanosleep(&req, NULL);
#elif HAVE_USLEEP
				usleep(1000);
#endif
			}//build line count
        }//not empty line
    }//while

	if (GetBuilderDirection() == TextProcess::Next)
		GetDocumentLineManager()->HasAllNextLines();
	else
		GetDocumentLineManager()->HasAllPreviousLines();

TPL_PRINTF("DocumentLineBuilder %d Stopped\n", GetBuilderDirection());

	return 1;
}

void TextProcess::Document::Impl::CDocumentLineBuilderImpl::Cancel()
{
	m_Cancel = 1;

	if (GetBuilderDirection() == TextProcess::Next)
		GetDocumentLineManager()->HasAllNextLines();
	else
		GetDocumentLineManager()->HasAllPreviousLines();
}

int TextProcess::Document::Impl::CDocumentLineBuilderImpl::IsEmptyLine(wxFileOffset offset, wxFileOffset length)
{
	if (length == 0)
		return 1;

    const wxByte * pBegin = GetDocumentFile()->GetBuffer() + offset;
    const wxByte * pEnd = pBegin + length;

	const wxByte * pSpace = reinterpret_cast<const wxByte *>(m_SpaceBuffer.data());
	const wxByte * pSpace2 = reinterpret_cast<const wxByte *>(m_Space2Buffer.data());
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
		else if (!memcmp(pBegin, pSpace2, m_Space2Length))
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
	size_t size = 0;
	wxMBConv * pEncoding = GetDocumentFile()->GetEncoding();
	m_SpaceBuffer = pEncoding->cWC2MB(wxT(" "), wxStrlen_(wxT(" ")), &size);
	m_SpaceLength = size;
	m_TabBuffer = pEncoding->cWC2MB(wxT("\t"), wxStrlen_(wxT("\t")), &size);
	m_TabLength = size;
	m_Space2Buffer = pEncoding->cWC2MB(wxT("　"), wxStrlen_(wxT("　")), &size);
	m_Space2Length = size;

	m_CRBuffer = pEncoding->cWC2MB(wxT("\n"), 1, &size);
	m_CRLength = size;
	m_LFBuffer = pEncoding->cWC2MB(wxT("\r"), 1, &size);
	m_LFLength = size;
}
