#pragma once

#include "LockStatus.h"

namespace TextProcess
{
	namespace Utils
	{
		class CReadWriteLock
		{
		public:
			CReadWriteLock(void);
			virtual ~CReadWriteLock(void);

		public:
			LockStatusEnum LockRead(int timeout = INFINITE);
			LockStatusEnum UnlockRead();
			LockStatusEnum LockWrite(int timeout = INFINITE);
			LockStatusEnum UnlockWrite();

		private:
			int m_nAccessCount;
			CCriticalSection m_AccessLock;
			CEvent m_hNotifyEvent;
		};
	}
}
