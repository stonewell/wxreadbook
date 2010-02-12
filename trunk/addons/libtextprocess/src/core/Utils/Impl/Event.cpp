#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::Utils::IEvent * TextProcess::Utils::IEvent::CreateEvent(int bManualReset, int bInitialized)
{
	return new TextProcess::Utils::Impl::CEvent(bManualReset, bInitialized);
}

TextProcess::Utils::IMultiWaitEvent * TextProcess::Utils::IMultiWaitEvent::CreateMultiWaitEvent()
{
	return new TextProcess::Utils::Impl::CMultiWaitEvent();
}

