#include "../TextProcess.h"
#include "../Impl/TextProcessImpl.h"

TextProcess::Impl::CLineBuilderImpl::CLineBuilderImpl(void) :
INIT_PROPERTY(BuilderDirection, TextProcess::Next)
,INIT_PROPERTY(BuildLineCount, 200)
,INIT_PROPERTY(WaitForLineAccessed, 1)
{
}

TextProcess::Impl::CLineBuilderImpl::~CLineBuilderImpl(void)
{
}
