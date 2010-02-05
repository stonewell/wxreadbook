#include <wx/wx.h>

#include "ReadWriteLock.h"

TextProcess::Utils::CReadWriteLock::CReadWriteLock(void) :
m_nAccessCount(0)
{
	InitializeCriticalSection(&m_AccessLock);
	m_hNotifyEvent = CreateEvent(NULL,
		TRUE, FALSE, NULL);
}

TextProcess::Utils::CReadWriteLock::~CReadWriteLock(void)
{
	DeleteCriticalSection(&m_AccessLock);
	CloseHandle(m_hNotifyEvent);
}

TextProcess::Utils::LockStatusEnum TextProcess::Utils::CReadWriteLock::LockRead(int timeout)
{
	EnterCriticalSection(&m_AccessLock);
	while (m_nAccessCount < 0)
	{
		LeaveCriticalSection(&m_AccessLock);

		switch(WaitForSingleObject(m_hNotifyEvent, timeout))
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			return TextProcess::Utils::Lock_TimeOut;
		case WAIT_ABANDONED:
			return TextProcess::Utils::Lock_Fail;
		}
	
		EnterCriticalSection(&m_AccessLock);
	}
	
	if (m_nAccessCount == 0)
		ResetEvent(m_hNotifyEvent);

	m_nAccessCount++;

	LeaveCriticalSection(&m_AccessLock);

	return TextProcess::Utils::Lock_OK;
}

TextProcess::Utils::LockStatusEnum TextProcess::Utils::CReadWriteLock::UnlockRead()
{
	EnterCriticalSection(&m_AccessLock);

	if (m_nAccessCount > 0)
		m_nAccessCount --;

	if (m_nAccessCount == 0)
	{
		SetEvent(m_hNotifyEvent);
	}

	LeaveCriticalSection(&m_AccessLock);

	return TextProcess::Utils::Lock_OK;
}

TextProcess::Utils::LockStatusEnum TextProcess::Utils::CReadWriteLock::LockWrite(int timeout)
{
	EnterCriticalSection(&m_AccessLock);

	while (m_nAccessCount != 0)
	{
		LeaveCriticalSection(&m_AccessLock);

		switch(WaitForSingleObject(m_hNotifyEvent, timeout))
		{
		case WAIT_OBJECT_0:
			break;
		case WAIT_TIMEOUT:
			return TextProcess::Utils::Lock_TimeOut;
		case WAIT_ABANDONED:
			return TextProcess::Utils::Lock_Fail;
		}
	
		EnterCriticalSection(&m_AccessLock);
	}
	
	m_nAccessCount--;
	ResetEvent(m_hNotifyEvent);

	LeaveCriticalSection(&m_AccessLock);

	return TextProcess::Utils::Lock_OK;
}

TextProcess::Utils::LockStatusEnum TextProcess::Utils::CReadWriteLock::UnlockWrite()
{
	EnterCriticalSection(&m_AccessLock);

	if (m_nAccessCount != 0)
		m_nAccessCount = 0;

	SetEvent(m_hNotifyEvent);

	LeaveCriticalSection(&m_AccessLock);

	return TextProcess::Utils::Lock_OK;
}
