#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::Document::Impl::CDocumentLineMatcherImpl::CDocumentLineMatcherImpl(void) :
INIT_PROPERTY(DocumentOffset, 0)
{
}

TextProcess::Document::Impl::CDocumentLineMatcherImpl::~CDocumentLineMatcherImpl(void)
{
}

int TextProcess::Document::Impl::CDocumentLineMatcherImpl::IsMatch(const ILine * pLine) const
{
	if (pLine->GetType() == TextProcess::ILine::DocumentLine)
	{
		const IDocumentLine * pDocLine = 
			dynamic_cast<const IDocumentLine *>(pLine);

		return pDocLine->GetOffset() <= GetDocumentOffset() &&
			pDocLine->GetOffset() + pDocLine->GetLength() > GetDocumentOffset();
	}

	return 0;
}

int TextProcess::Document::Impl::CDocumentLineMatcherImpl::IsBeforeLine(const ILine * pLine) const
{
	if (pLine->GetType() == TextProcess::ILine::DocumentLine)
	{
		const IDocumentLine * pDocLine = 
			dynamic_cast<const IDocumentLine *>(pLine);

		return pDocLine->GetOffset() > GetDocumentOffset();
	}

	return 0;
}

int TextProcess::Document::Impl::CDocumentLineMatcherImpl::IsAfterLine(const ILine * pLine) const
{
	if (pLine->GetType() == TextProcess::ILine::DocumentLine)
	{
		const IDocumentLine * pDocLine = 
			dynamic_cast<const IDocumentLine *>(pLine);

		return pDocLine->GetOffset() + pDocLine->GetLength() <= GetDocumentOffset();
	}

	return 0;
}


int TextProcess::Document::Impl::CDocumentLineMatcherImpl::Compare(const ILine * pLine) const
{
	if (IsMatch(pLine)) return 0;
	if (IsAfterLine(pLine)) return 1;
	
	return -1;
}

