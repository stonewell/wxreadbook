#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::Document::Impl::CDocumentLineImpl::CDocumentLineImpl(void) :
TextProcess::Impl::CLineImpl(TextProcess::ILine::DocumentLine),
INIT_PROPERTY(DocumentFile, NULL)
,m_DecodedBuffer(NULL)
{
}

TextProcess::Document::Impl::CDocumentLineImpl::~CDocumentLineImpl(void)
{
}

void TextProcess::Document::Impl::CDocumentLineImpl::GetData(int nOffset, int nLength,
															 wxChar ** ppBuf, int * ppBufLen)
{
	if (nOffset >= m_nDecodedLength)
	{
		*ppBuf = NULL;

		*ppBufLen = 0;

		return;
	}

	if (nOffset + nLength > m_nDecodedLength)
		nLength = m_nDecodedLength - nOffset;

	wxChar * pDataBuf = GetDecodedBuffer();

	*ppBuf = pDataBuf + nOffset;
	*ppBufLen = nLength;
}

wxString TextProcess::Document::Impl::CDocumentLineImpl::GetData(int nOffset, int nLength)
{
    wxChar * pBuf = NULL;
    int pBufLen = 0;

    GetData(nOffset, nLength, &pBuf, &pBufLen);

    if (!pBufLen) return wxEmptyString;

    return wxString(pBuf, pBufLen);
}

wxChar * TextProcess::Document::Impl::CDocumentLineImpl::GetDecodedBuffer()
{
	if (m_DecodedBuffer) return m_DecodedBuffer;

	TextProcess::Utils::CCriticalSectionAccessor accessor(&m_CriticalSection);

	if (m_DecodedBuffer) return m_DecodedBuffer;

	m_DecodedBuffer = GetDocumentFile()->DecodeData(GetOffset(), GetLength(), m_nDecodedLength);

	return m_DecodedBuffer;
}

