#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::View::Impl::CViewLineImpl::CViewLineImpl(void) :
CLineImpl(ILine::ViewLine)
, INIT_PROPERTY(DocumentLine, NULL)
{
}

TextProcess::View::Impl::CViewLineImpl::~CViewLineImpl(void)
{
}
