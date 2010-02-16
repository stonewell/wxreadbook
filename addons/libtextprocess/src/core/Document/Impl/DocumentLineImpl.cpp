#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::Document::Impl::CDocumentLineImpl::CDocumentLineImpl(void) :
TextProcess::Impl::CLineImpl(TextProcess::ILine::DocumentLine),
INIT_PROPERTY(DocumentFile, NULL)
,m_DecodedBuffer(NULL)
,m_nDecodedLength(0)
{
}

TextProcess::Document::Impl::CDocumentLineImpl::~CDocumentLineImpl(void)
{
}

void TextProcess::Document::Impl::CDocumentLineImpl::GetData(wxFileOffset nOffset, wxFileOffset nLength,
															 wxChar ** ppBuf, wxFileOffset * ppBufLen)
{
	if (nOffset >= GetDecodedLength())
	{
		*ppBuf = NULL;

		*ppBufLen = 0;

		return;
	}

	if (nOffset + nLength > GetDecodedLength())
		nLength = GetDecodedLength() - nOffset;

	wxChar * pDataBuf = GetDecodedBuffer();

	*ppBuf = pDataBuf + nOffset;
	*ppBufLen = nLength;

	AccessLine();
}

wxString TextProcess::Document::Impl::CDocumentLineImpl::GetData(wxFileOffset nOffset, wxFileOffset nLength)
{
    wxChar * pBuf = NULL;
    wxFileOffset pBufLen = 0;

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

