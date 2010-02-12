#pragma once

namespace TextProcess
{
	namespace Utils
	{
		enum LockStatusEnum
        {
            Lock_OK,
            Lock_Fail,
            Lock_TimeOut
        };

		class IReadWriteLock
		{
		public:
			DECLARE_TPL_INTERFACE(IReadWriteLock);

		public:
			static IReadWriteLock * CreateReadWriteLock();

			virtual LockStatusEnum LockRead(int timeout = INFINITE) = 0;
			virtual LockStatusEnum UnlockRead() = 0;
			virtual LockStatusEnum LockWrite(int timeout = INFINITE) = 0;
			virtual LockStatusEnum UnlockWrite() = 0;
		};

		class CReadWriteLockAccessor
		{
		private:
			int m_nRead;
			IReadWriteLock * m_pLock;
		public:
			CReadWriteLockAccessor(IReadWriteLock * pLock, int read = 1) :
			  m_nRead(read), m_pLock(pLock)
			{
				if (read)
					pLock->LockRead();
				else
					pLock->LockWrite();
			}

			~CReadWriteLockAccessor(void)
			{
				if (m_nRead)
					m_pLock->UnlockRead();
				else
					m_pLock->UnlockWrite();
			}
		};
	}
}

