#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::Utils::Impl::CReadWriteLock::CReadWriteLock(void) :
m_nAccessCount(0)
{
}

TextProcess::Utils::Impl::CReadWriteLock::~CReadWriteLock(void)
{
}

TextProcess::Utils::LockStatusEnum TextProcess::Utils::Impl::CReadWriteLock::LockRead(int timeout)
{
	m_AccessLock.Enter();
	while (m_nAccessCount < 0)
	{
		m_AccessLock.Leave();

		switch(m_hNotifyEvent.Wait(timeout))
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			return TextProcess::Utils::Lock_TimeOut;
		case WAIT_ABANDONED:
			return TextProcess::Utils::Lock_Fail;
		}

		m_AccessLock.Enter();
	}

	if (m_nAccessCount == 0)
		m_hNotifyEvent.Reset();

	m_nAccessCount++;

	m_AccessLock.Leave();

	return TextProcess::Utils::Lock_OK;
}

TextProcess::Utils::LockStatusEnum TextProcess::Utils::Impl::CReadWriteLock::UnlockRead()
{
	TextProcess::Utils::CCriticalSectionAccessor a(&m_AccessLock);

	if (m_nAccessCount > 0)
		m_nAccessCount --;

	if (m_nAccessCount == 0)
	{
		m_hNotifyEvent.Set();
	}

	return TextProcess::Utils::Lock_OK;
}

TextProcess::Utils::LockStatusEnum TextProcess::Utils::Impl::CReadWriteLock::LockWrite(int timeout)
{
	m_AccessLock.Enter();

	while (m_nAccessCount != 0)
	{
		m_AccessLock.Leave();

		switch(m_hNotifyEvent.Wait(timeout))
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			return TextProcess::Utils::Lock_TimeOut;
		case WAIT_ABANDONED:
			return TextProcess::Utils::Lock_Fail;
		}

		m_AccessLock.Enter();
	}

	m_nAccessCount--;
	m_hNotifyEvent.Reset();

	m_AccessLock.Leave();

	return TextProcess::Utils::Lock_OK;
}

TextProcess::Utils::LockStatusEnum TextProcess::Utils::Impl::CReadWriteLock::UnlockWrite()
{
	TextProcess::Utils::CCriticalSectionAccessor a(&m_AccessLock);

	if (m_nAccessCount != 0)
		m_nAccessCount = 0;

	m_hNotifyEvent.Set();

	return TextProcess::Utils::Lock_OK;
}

TextProcess::Utils::IReadWriteLock * TextProcess::Utils::IReadWriteLock::CreateReadWriteLock()
{
	return new TextProcess::Utils::Impl::CReadWriteLock();
}

