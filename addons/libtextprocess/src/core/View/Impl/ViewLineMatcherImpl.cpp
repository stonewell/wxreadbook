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
        const IViewLine * pViewLine = reinterpret_cast<const IViewLine *>(pLine);

        return (pViewLine->GetDocumentLine()->GetOffset() <= GetDocumentLineOffset() &&
            pViewLine->GetDocumentLine()->GetOffset() + pViewLine->GetDocumentLine()->GetLength() > GetDocumentLineOffset() &&
            pViewLine->GetOffset() <= GetViewLineOffset() &&
            pViewLine->GetOffset() + pViewLine->GetLength() > GetViewLineOffset());
    }

	return 0;
}
