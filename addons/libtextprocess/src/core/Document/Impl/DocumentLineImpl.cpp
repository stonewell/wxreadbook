#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::Document::Impl::CDocumentLineImpl::CDocumentLineImpl(void) :
TextProcess::Impl::CLineImpl(TextProcess::ILine::DocumentLine),
INIT_PROPERTY(DocumentFile, NULL)
{
}

TextProcess::Document::Impl::CDocumentLineImpl::~CDocumentLineImpl(void)
{
}

void TextProcess::Document::Impl::CDocumentLineImpl::GetData(int nOffset, int nLength, wxChar * pBuf, int * pBuf_Size) const
{
}

const wxString & TextProcess::Document::Impl::CDocumentLineImpl::GetData(int nOffset, int nLength) const
{
	return wxT("");
}

