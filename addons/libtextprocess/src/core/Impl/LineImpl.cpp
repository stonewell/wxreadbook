#include "../TextProcess.h"
#include "TextProcessImpl.h"

TextProcess::Impl::CLineImpl::CLineImpl(LineTypeEnum nLineType) :
INIT_PROPERTY(Type, nLineType)
,INIT_PROPERTY(Offset, 0)
,INIT_PROPERTY(Length, 0)
,INIT_PROPERTY(Index, 0)
{
}

TextProcess::Impl::CLineImpl::~CLineImpl(void)
{
}
