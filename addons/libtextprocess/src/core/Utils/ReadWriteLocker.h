#pragma once

namespace TextProcess
{
	namespace Utils
	{
		class CReadWriteLock;
		class CReadWriteLocker
		{
		private:
			int m_nRead;
			CReadWriteLock * m_pLock;
		public:
			CReadWriteLocker(CReadWriteLock * pLock, int read = 1) :
			  m_nRead(read), m_pLock(pLock)
			{
				if (read)
					pLock->LockRead();
				else
					pLock->LockWrite();
			}

			virtual ~CReadWriteLocker(void)
			{
				if (m_nRead)
					m_pLock->UnlockRead();
				else
					m_pLock->UnlockWrite();
			}
		};
	}
}

