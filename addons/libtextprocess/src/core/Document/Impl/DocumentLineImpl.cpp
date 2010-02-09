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
	if (nOffset >= GetLength())
	{
		*ppBuf = NULL;

		*ppBufLen = 0;

		return;
	}

	if (nOffset + nLength > GetLength())
		nLength = GetLength() - nOffset;

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

	m_DecodedBuffer = GetDocumentFile()->DecodeData(GetOffset(), GetLength());

	return m_DecodedBuffer;
}

