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

int TextProcess::Impl::CLineImpl::WaitForAccessing(int timeout)
{
	return m_AccessEvent.Wait(timeout);
}

void TextProcess::Impl::CLineImpl::AccessLine()
{
	m_AccessEvent.Set();
}

void TextProcess::Impl::CLineImpl::ClearAccess()
{
	m_AccessEvent.Reset();
}

