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

TextProcess::Document::Impl::CDocumentLineBuilderImpl::CDocumentLineBuilderImpl(
    void) :
	INIT_PROPERTY(DocumentOffset, 0),
	INIT_PROPERTY(DocumentLineManager, NULL),
	INIT_PROPERTY(DocumentFile, NULL),
	INIT_PROPERTY(Cancel, 0)
{
}

TextProcess::Document::Impl::CDocumentLineBuilderImpl::~CDocumentLineBuilderImpl(
    void) {
}

int TextProcess::Document::Impl::CDocumentLineBuilderImpl::BuildLines() {
	InitBuffers();

	const wxByte * pFileBegin = GetDocumentFile()->GetBuffer();
	const wxByte * pFileEnd = pFileBegin + GetDocumentFile()->GetLength();
	const wxByte * pStartPos = pFileBegin + GetDocumentOffset();
	const wxByte * pEndPos = 0;
	const wxByte * pCR = reinterpret_cast<const wxByte *> (m_CRBuffer.data());
	const wxByte * pLF = reinterpret_cast<const wxByte *> (m_LFBuffer.data());
	const wxByte * pCREnd = pCR + m_CRLength;
	const wxByte * pLFEnd = pLF + m_LFLength;
	const wxByte * pchEOL = NULL;
	const wxByte * pchEOL2 = NULL;

	if (pStartPos > pFileEnd)
		pStartPos = pFileEnd;

	if (pStartPos < pFileBegin)
		pStartPos = pFileBegin;

	std::shared_ptr<TextProcess::Document::IDocumentLineMatcher> pMatcher(
        CDocumentObjectFactory::CreateLineMatcher(GetDocumentOffset()));

	TPL_PRINTF("&&&&&&&& %d %x\n", GetDocumentOffset(),GetDocumentOffset());

	TextProcess::Document::IDocumentLine * pLastLine =
			GetDocumentLineManager()->FindLine(pMatcher.get(), 0);

	if (pLastLine != NULL) {
		TPL_PRINTF("last line is not null 1:%d\n", pStartPos - pFileBegin);
		pStartPos = pFileBegin + pLastLine->GetOffset();
		TPL_PRINTF("last line is not null 2:%d\n", pStartPos - pFileBegin);
	} else {
		TPL_PRINTF("last line is null 1:%d %x %x %d %d\n",
                   pStartPos - pFileBegin,
                   *pCR, *pLF, m_CRLength, m_LFLength);

		bool cr = true;
		pchEOL = std::find_end(pFileBegin, pStartPos, pCR, pCREnd);
		pchEOL2 = std::find_end(pFileBegin, pStartPos, pLF, pLFEnd);

		TPL_PRINTF("last line is null 2:%d, %d %d\n", pStartPos - pFileBegin,
                   pchEOL - pFileBegin, pchEOL2 - pFileBegin);

		if (pchEOL2 < pStartPos && pchEOL2 > pchEOL) {
			pchEOL = pchEOL2;
			cr = false;
		}

		TPL_PRINTF("last line is null 2:%d, %d %d\n", pStartPos - pFileBegin,
                   pchEOL - pFileBegin, pchEOL2 - pFileBegin);

		if (pStartPos != pchEOL) {
			if (GetBuilderDirection() == TextProcess::Next) {
				if (cr)
					pStartPos = pchEOL + m_CRLength;
				else if (pchEOL + m_LFLength + m_CRLength < pFileEnd && !memcmp(pchEOL
                                                                                + m_LFLength, pCR, m_CRLength)) {
					pStartPos = pchEOL + m_LFLength + m_CRLength;
				} else {
					pStartPos = pchEOL + m_LFLength;
				}
			} else {
				if (cr && !memcmp(pchEOL - m_LFLength, pLF, m_LFLength)) {
					pStartPos = pchEOL - m_LFLength;
				} else {
					pStartPos = pchEOL;
				}
			}
		} else {
			pStartPos = pFileBegin;
		}
	}

	if (GetBuilderDirection() == TextProcess::Next) {
		pEndPos = pFileEnd;
	} else {
		pEndPos = pFileBegin;

		std::swap(pEndPos, pStartPos);
	}

	TPL_PRINTF("DocumentLineBuilder %d started at %ld\n", GetBuilderDirection(), pStartPos - pFileBegin);

	TPL_PRINTF("^^^&&&&&&&& %d.%d %d\n", GetBuilderDirection(), GetDocumentOffset(), pStartPos - pFileBegin);

	wxUint32 nBuildLineCount = GetBuildLineCount();

	if (!nBuildLineCount) {
		TPL_PRINTF("DocumentLineBuilder %d BuildLineCount is zero\n", GetBuilderDirection());
	}

	while (!m_Cancel && pStartPos < pEndPos) {
		wxFileOffset length = 0;
		wxFileOffset offset = 0;

		if (GetBuilderDirection() == TextProcess::Next) {
			bool cr = true;
			pchEOL = std::search(pStartPos, pEndPos, pCR, pCREnd);
			pchEOL2 = std::search(pStartPos, pEndPos, pLF, pLFEnd);


			//sequence search, choose the less one
			if (pchEOL2 < pchEOL) {
				pchEOL = pchEOL2;
				cr = false;
			}

			length = pchEOL - pStartPos;
			offset = pStartPos - pFileBegin;

			if (cr) {
				pStartPos = pchEOL + m_CRLength;
			} else {
				pStartPos = pchEOL + m_LFLength;

				if (pStartPos + m_CRLength <= pEndPos) {
					if (!memcmp(pStartPos, pCR, m_CRLength))
						pStartPos += m_CRLength;
				}
			}

			if (cr && length >= m_LFLength) {
				if (!memcmp(pFileBegin + offset + length - m_LFLength, pLF, m_LFLength)) {
					length -= m_LFLength;
				}
			}
		} else {
			bool cr = true;
			pchEOL = std::find_end(pStartPos, pEndPos, pCR, pCREnd);
			pchEOL2 = std::find_end(pStartPos, pEndPos, pLF, pLFEnd);

            TPL_PRINTF("cr. offset = %p %p %p %d %d\n", pEndPos, pchEOL, pchEOL2, pEndPos - pchEOL, m_CRLength);


			//reverse search, so choose the bigger one.
			if (pchEOL2 < pEndPos && pchEOL2 > pchEOL) {
				cr = false;
				pchEOL = pchEOL2;
			}

			if (pchEOL == pEndPos) {
				offset = 0;
				length = pEndPos - pFileBegin + 1;
			} else {
				if (cr) {
					offset = (pchEOL + m_CRLength) - pFileBegin;
					length = (pEndPos - (pchEOL + m_CRLength)) + 1;
                    int ll = length;
                    TPL_PRINTF("length=%d,%d\n", ll, pEndPos - (pchEOL + m_CRLength) + 1);

                    TPL_PRINTF("cr. @@@@@@@ offset = %p %p %p %d %d\n", pEndPos, pchEOL, pchEOL2, pEndPos - pchEOL, m_CRLength);
                    TPL_PRINTF("cr. offset = %d %d %d\n", offset, length, (pEndPos - (pchEOL + m_CRLength)) + 1);

					if (pchEOL - m_LFLength >= pStartPos) {
						if (!memcmp(pchEOL - m_LFLength, pLF, m_LFLength)) {
							pchEOL -= m_LFLength;
						}
					}
				} else {
					offset = (pchEOL + m_LFLength) - pFileBegin;
					length = (pEndPos - (pchEOL + m_LFLength)) + 1;
                    TPL_PRINTF("lf. offset = %d %d\n", offset, length);

					while (length >= m_CRLength) {
						if (!memcmp(pFileBegin + offset, pCR, m_CRLength)) {
							length -= m_LFLength;
							offset += m_CRLength;
						} else {
							break;
						}
					}
				}

			}

			pEndPos = pchEOL - 1;
		}

		if (!m_Cancel && !IsEmptyLine(offset, length)) {
			TextProcess::Document::IDocumentLine * pDocLine = NULL;

            std::shared_ptr<TextProcess::Document::IDocumentLineMatcher> pMatcherTmp(
                CDocumentObjectFactory::CreateLineMatcher(offset));

			pDocLine = GetDocumentLineManager()->FindLine(pMatcherTmp.get(), 0);

			if (pDocLine == NULL) {
                pDocLine = CDocumentObjectFactory::CreateDocumentLine(offset, length,
                                                                      GetDocumentFile());

                TPL_PRINTF("Add Next Line &&&&&&&& %d.%d %d\n", GetBuilderDirection(), GetDocumentOffset(), offset);

                if (GetBuilderDirection() == TextProcess::Next)
                    GetDocumentLineManager()->AddNextLine(pDocLine, pLastLine);
                else
                    GetDocumentLineManager()->AddPrevLine(pDocLine, pLastLine);
			}

			pLastLine = pDocLine;

			if (nBuildLineCount > 0)
				nBuildLineCount--;

			if (!nBuildLineCount) {
				if (!GetWaitForLineAccessed())
					return 2;

				TPL_PRINTF("DocumentLineBuilder %d Wait for last line access offset=%ld\n", GetBuilderDirection(), pLastLine->GetOffset());
				while (!m_Cancel) {
					if (pLastLine->WaitForAccessing(500) != WAIT_TIMEOUT)
						break;
				}

				TPL_PRINTF("DocumentLineBuilder %d last line waited offset=%ld\n", GetBuilderDirection(), pLastLine->GetOffset());
				nBuildLineCount = GetBuildLineCount();
			} else {
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

void TextProcess::Document::Impl::CDocumentLineBuilderImpl::Cancel() {
	m_Cancel = 1;

	if (GetBuilderDirection() == TextProcess::Next)
		GetDocumentLineManager()->HasAllNextLines();
	else
		GetDocumentLineManager()->HasAllPreviousLines();
}

int TextProcess::Document::Impl::CDocumentLineBuilderImpl::IsEmptyLine(
    wxFileOffset offset, wxFileOffset length) {
	if (length == 0)
		return 1;

	const wxByte * pBegin = GetDocumentFile()->GetBuffer() + offset;
	const wxByte * pEnd = pBegin + length;

	const wxByte * pSpace =
			reinterpret_cast<const wxByte *> (m_SpaceBuffer.data());
	const wxByte * pSpace2 =
			reinterpret_cast<const wxByte *> (m_Space2Buffer.data());
	const wxByte * pTab = reinterpret_cast<const wxByte *> (m_TabBuffer.data());

	while (pBegin < pEnd) {
		if (m_SpaceLength > 0 && !memcmp(pBegin, pSpace, m_SpaceLength)) {
			pBegin += m_SpaceLength;
		} else if (m_TabLength > 0 && !memcmp(pBegin, pTab, m_TabLength)) {
			pBegin += m_TabLength;
		} else if (m_Space2Length > 0 && !memcmp(pBegin, pSpace2, m_Space2Length)) {
			pBegin += m_Space2Length;
		} else {
			return 0;
		}
	}

	return 1;
}

void TextProcess::Document::Impl::CDocumentLineBuilderImpl::InitBuffers() {
	size_t size = 0;

#if (wxMAJOR_VERSION > 2) || (wxMAJOR_VERSION >= 2 && wxMINOR_VERSION >= 9)
#define wxStrlen_ wxStrlen
#endif
	wxMBConv * pEncoding = GetDocumentFile()->GetEncoding();
	m_SpaceBuffer = pEncoding->cWC2MB(wxT(" "), wxStrlen_(wxT(" ")), &size);
	m_SpaceLength = size;
	m_TabBuffer = pEncoding->cWC2MB(wxT("\t"), wxStrlen_(wxT("\t")), &size);
	m_TabLength = size;
	m_Space2Buffer = pEncoding->cWC2MB(wxT("\xe380\x800a"), wxStrlen_(
        wxT("\xe380\x800a")), &size);
	m_Space2Length = size;

	m_CRBuffer = pEncoding->cWC2MB(wxT("\n"), 1, &size);
	m_CRLength = size;
	m_LFBuffer = pEncoding->cWC2MB(wxT("\r"), 1, &size);
	m_LFLength = size;

#if wxMAJOR_VERSION >= 2 && wxMINOR_VERSION >= 9
    p	#undef wxStrlen_
#endif
            }
