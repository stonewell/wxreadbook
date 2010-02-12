#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::Utils::ICriticalSection *  TextProcess::Utils::ICriticalSection::CreateCriticalSection();
{
	return new TextProcess::Utils::Impl::CCriticalSection();
}

