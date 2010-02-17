#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::View::Impl::CViewLineMatcherImpl::CViewLineMatcherImpl(void) :
INIT_PROPERTY(DocumentLineOffset, 0)
, INIT_PROPERTY(ViewLineOffset, 0)
{
}

TextProcess::View::Impl::CViewLineMatcherImpl::~CViewLineMatcherImpl(void)
{
}

int TextProcess::View::Impl::CViewLineMatcherImpl::IsMatch(const ILine * pLine) const
{
	if (pLine->GetType() == ILine::ViewLine)
    {
        const IViewLine * pViewLine = dynamic_cast<const IViewLine *>(pLine);

        return (pViewLine->GetDocumentLine()->GetOffset() <= GetDocumentLineOffset() &&
            pViewLine->GetDocumentLine()->GetOffset() + pViewLine->GetDocumentLine()->GetLength() > GetDocumentLineOffset() &&
            pViewLine->GetOffset() <= GetViewLineOffset() &&
            pViewLine->GetOffset() + pViewLine->GetLength() > GetViewLineOffset());
    }

	return 0;
}

int TextProcess::View::Impl::CViewLineMatcherImpl::IsBeforeLine(const ILine * pLine) const
{
	if (pLine->GetType() == ILine::ViewLine)
    {
        const IViewLine * pViewLine = dynamic_cast<const IViewLine *>(pLine);

        return  pViewLine->GetDocumentLine()->GetOffset() > GetDocumentLineOffset() ||
			(pViewLine->GetDocumentLine()->GetOffset() <= GetDocumentLineOffset() &&
            pViewLine->GetDocumentLine()->GetOffset() + pViewLine->GetDocumentLine()->GetLength() > GetDocumentLineOffset() &&
            pViewLine->GetOffset() > GetViewLineOffset());
    }

	return 0;
}

int TextProcess::View::Impl::CViewLineMatcherImpl::IsAfterLine(const ILine * pLine) const
{
	if (pLine->GetType() == ILine::ViewLine)
    {
        const IViewLine * pViewLine = dynamic_cast<const IViewLine *>(pLine);

        return pViewLine->GetDocumentLine()->GetOffset() + pViewLine->GetDocumentLine()->GetLength() <= GetDocumentLineOffset() ||
			(pViewLine->GetDocumentLine()->GetOffset() <= GetDocumentLineOffset() &&
            pViewLine->GetDocumentLine()->GetOffset() + pViewLine->GetDocumentLine()->GetLength() > GetDocumentLineOffset() &&
            pViewLine->GetOffset() + pViewLine->GetLength() <= GetViewLineOffset());
    }

	return 0;
}

int TextProcess::View::Impl::CViewLineMatcherImpl::Compare(const ILine * pLine) const
{
	if (IsMatch(pLine)) return 0;

	if (IsAfterLine(pLine)) return 1;

	return -1;
}

