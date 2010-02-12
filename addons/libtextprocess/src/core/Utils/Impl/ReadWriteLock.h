#pragma once

namespace TextProcess
{
	namespace Utils
	{
		namespace Impl
		{
		class CReadWriteLock
			: public IReadWriteLock
		{
		public:
			CReadWriteLock(void);
			virtual ~CReadWriteLock(void);

		public:
			virtual LockStatusEnum LockRead(int timeout = INFINITE);
			virtual LockStatusEnum UnlockRead();
			virtual LockStatusEnum LockWrite(int timeout = INFINITE);
			virtual LockStatusEnum UnlockWrite();

		private:
			int m_nAccessCount;
			CCriticalSection m_AccessLock;
			CEvent m_hNotifyEvent;
		};
		}
	}
}
