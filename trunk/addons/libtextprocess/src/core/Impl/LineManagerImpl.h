#pragma once

#include "../Utils/ReadWriteLock.h"
#include "../Utils/ReadWriteLocker.h"

namespace TextProcess
{
	namespace Impl
	{
		template<class T, class U>	
		class CLineManagerImpl
		{
			class CCriticalSectionAccessor
			{
			private:
				LPCRITICAL_SECTION m_pSec;
			public:
				CCriticalSectionAccessor(LPCRITICAL_SECTION pSec) :
				  m_pSec(pSec)
				  {
					  EnterCriticalSection(pSec);
				  }

				  ~CCriticalSectionAccessor()
				  {
					  LeaveCriticalSection(m_pSec);
				  }
			};

			class CMultiWaitEvent //Auto Reset wait ref count == 0
			{
			private:
				int m_nWait;
				HANDLE m_nEvent;
				CRITICAL_SECTION m_AccessLock;

			public:
				CMultiWaitEvent() :
				  m_nWait(0)
				{
					InitializeCriticalSection(&m_AccessLock);
					m_nEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
				}

				~CMultiWaitEvent()
				{
					CloseHandle(m_nEvent);
					DeleteCriticalSection(&m_AccessLock);
				}

				void NotifyAll()
				{
					CCriticalSectionAccessor a(&m_AccessLock);
					SetEvent(m_nEvent);
				}

				void Wait()
				{
					{
						CCriticalSectionAccessor a(&m_AccessLock);

						m_nWait++;
					}

					WaitForSingleObject(m_nEvent, INFINITE);

					{
						CCriticalSectionAccessor a(&m_AccessLock);
						m_nWait--;
						
						if (!m_nWait)
							ResetEvent(m_nEvent);
					}
				}
			};

			class CLineEntry
			{
			public:
				CLineEntry()
				{
					m_hNextWaitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
					m_hPrevWaitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
					m_hDataWaitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

					InitializeCriticalSection(&m_AccessLock);
				}

				~CLineEntry()
				{
					CloseHandle(m_hNextWaitEvent);
					CloseHandle(m_hPrevWaitEvent);
					CloseHandle(m_hDataWaitEvent);

					DeleteCriticalSection(&m_AccessLock);

					if (m_pLine != NULL) delete m_pLine;
				}
			private:
				T * m_pLine;
				CLineEntry * m_pPrevEntry;
				CLineEntry * m_pNextEntry;

				CRITICAL_SECTION m_AccessLock;

				HANDLE m_hNextWaitEvent;
				HANDLE m_hPrevWaitEvent;
				HANDLE m_hDataWaitEvent;

			public:
				void NotifyNextChange()
				{
					SetEvent(m_hNextWaitEvent);
				}

				void NotifyPrevChange()
				{
					SetEvent(m_hPrevWaitEvent);
				}

				void NotifyDataChange()
				{
					SetEvent(m_hDataWaitEvent);
				}

				T * GetLine() 
				{ 
					CCriticalSectionAccessor accessor(&m_AccessLock); 
					return m_pLine; 
				}

				void SetLine(T * pLine) 
				{ 
					CCriticalSectionAccessor accessor(&m_AccessLock); 
					m_pLine = pLine; 
					NotifyDataChange();
				}

				void WaitData()
				{
					WaitForSingleObject(m_hDataWaitEvent, INFINITE);
				}

				CLineEntry * GetNext() { CCriticalSectionAccessor accessor(&m_AccessLock); return m_pNextEntry; }
				void SetNext(CLineEntry * pNext)
				{
					CCriticalSectionAccessor accessor(&m_AccessLock); 
					m_pNextEntry = pNext;
					NotifyNextChange();
				}

				void WaitNext()
				{
					WaitForSingleObject(m_hNextWaitEvent, INFINITE);
				}

				CLineEntry * GetPrev() { CCriticalSectionAccessor accessor(&m_AccessLock); return m_pPrevEntry; }
				void SetPrev(CLineEntry * pPrev)
				{
					CCriticalSectionAccessor accessor(&m_AccessLock); 
					m_pPrevEntry = pPrev;
					NotifyPrevChange();
				}

				void WaitPrev()
				{
					WaitForSingleObject(m_hPrevWaitEvent, INFINITE);
				}
			};

		private:
			TextProcess::Utils::CReadWriteLock m_ReadWriteLock;

			int m_bHasAllLines;
			int m_bClearing;

			CLineEntry * m_pHeaderEntry;
			CLineEntry * m_pTailEntry;

			CMultiWaitEvent m_NewDataEvent;

			CLineEntry * FindEntry(T * pLine)
			{
				CLineEntry * pEntry = m_pHeaderEntry;

				while(pEntry != NULL)
				{
					if (pEntry->GetLine() == pLine)
						return pEntry;

					pEntry = pEntry->GetNext();
				}

				return NULL;
			}

			void WaitNewEntry()
			{
				m_NewDataEvent.Wait();
			}

			void NotifyNewEntryCome()
			{
				m_NewDataEvent.NotifyAll();
			}
		public:
			CLineManagerImpl(void) :
				m_bHasAllLines(0),
				m_bClearing(0),
				m_pHeaderEntry(NULL),
				m_pTailEntry(NULL)
			{
			}

			virtual ~CLineManagerImpl(void)
			{
				Clear();
			}

		public:
			virtual T * FindLine(U * pMatcher, int wait = 1)
			{
				m_ReadWriteLock.LockRead();

				while(true)
				{
					CLineEntry * pEntry = m_pHeaderEntry;

					while (pEntry != NULL)
					{
						T * pLine = pEntry->GetLine();

						if (pLine != NULL && pMatcher->IsMatch(pLine))
						{
							m_ReadWriteLock.UnlockRead();
							return pLine;
						}

						pEntry = pEntry->GetNext();
					}

					if (!IsHasAllLines() && wait)
					{
						m_ReadWriteLock.UnlockRead();
						WaitNewEntry();

						if (m_bClearing)
							return NULL;

						m_ReadWriteLock.LockRead();
					}
					else
					{
						break;
					}
				}

				m_ReadWriteLock.UnlockRead();

				return NULL;
			}

			virtual T * GetNextLine(T * pLine, int wait = 1)
			{
				m_ReadWriteLock.LockRead();

				CLineEntry * pEntry = FindEntry(pLine);

				if (pEntry == NULL) 
				{
					m_ReadWriteLock.UnlockRead();
					return NULL;
				}

				while(true)
				{
					CLineEntry * pNextEntry = pEntry->GetNext();

					if (pNextEntry != NULL) 
					{
						while(true)
						{
							T * pNextLine = pNextEntry->GetLine();

							if (pNextLine != NULL) 
							{
								m_ReadWriteLock.UnlockRead();

								return pNextLine;
							}

							m_ReadWriteLock.UnlockRead();

							if (!IsHasAllLines() && wait)
								pNextEntry->WaitData();
							else
								return NULL;

							if (m_bClearing)
								return NULL;

							m_ReadWriteLock.LockRead();
						}
					}

					m_ReadWriteLock.UnlockRead();
					
					if (!IsHasAllLines() && wait)
						pEntry->WaitNext();
					else
						return NULL;

					if (m_bClearing)
						return NULL;

					m_ReadWriteLock.LockRead();
				}
			}

			virtual T * GetPrevLine(T * pLine, int wait = 1)
			{
				m_ReadWriteLock.LockRead();

				CLineEntry * pEntry = FindEntry(pLine);

				if (pEntry == NULL) 
				{
					m_ReadWriteLock.UnlockRead();
					return NULL;
				}

				while(true)
				{
					CLineEntry * pPrevEntry = pEntry->GetPrev();

					if (pPrevEntry != NULL) 
					{
						while(true)
						{
							T * pPrevLine = pPrevEntry->GetLine();

							if (pPrevLine != NULL) 
							{
								m_ReadWriteLock.UnlockRead();

								return pPrevLine;
							}

							m_ReadWriteLock.UnlockRead();

							if (!IsHasAllLines() && wait)
								pPrevEntry->WaitData();
							else
								return NULL;

							if (m_bClearing)
								return NULL;

							m_ReadWriteLock.LockRead();
						}
					}

					m_ReadWriteLock.UnlockRead();
					
					if (!IsHasAllLines() && wait)
						pEntry->WaitPrev();
					else
						return NULL;

					if (m_bClearing)
						return NULL;

					m_ReadWriteLock.LockRead();
				}
			}

			virtual void AddNextLine(T * pNextLine, T * pLine)
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock, 0);

				CLineEntry * pEntry = NULL;

				if (pLine != NULL)
				{
					pEntry = FindEntry(pLine);
				}

				if (pEntry == NULL)
				{
					pEntry = m_pTailEntry;
				}

				if (pEntry == NULL)
				{
					pEntry = m_pHeaderEntry = m_pTailEntry = new CLineEntry();

					pEntry->SetLine(pNextLine);
					pEntry->SetPrev(NULL);
					pEntry->SetNext(NULL);
				}
				else
				{
					CLineEntry * tmpEntry = new CLineEntry();
					CLineEntry * pNextEntry = pEntry->GetNext();

					tmpEntry->SetLine(pNextLine);

					tmpEntry->SetNext(pNextEntry);

					if (pNextEntry != NULL)
					{
						pNextEntry->SetPrev(tmpEntry);
					}
					else
					{
						m_pTailEntry = tmpEntry;
					}
					
					tmpEntry->SetPrev(pEntry);
					pEntry->SetNext(tmpEntry);
				}
			}

			virtual void AddPrevLine(T * pPrevLine, T * pLine)
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock, 0);

				CLineEntry * pEntry = NULL;

				if (pLine != NULL)
				{
					pEntry = FindEntry(pLine);
				}

				if (pEntry == NULL)
				{
					pEntry = m_pHeaderEntry;
				}

				if (pEntry == NULL)
				{
					pEntry = m_pHeaderEntry = m_pTailEntry = new CLineEntry();

					pEntry->SetLine(pPrevLine);
					pEntry->SetPrev(NULL);
					pEntry->SetNext(NULL);
				}
				else
				{
					CLineEntry * tmpEntry = new CLineEntry();
					CLineEntry * pPrevEntry = pEntry->GetPrev();

					tmpEntry->SetLine(pPrevLine);

					tmpEntry->SetPrev(pPrevEntry);

					if (pPrevEntry != NULL)
						pPrevEntry->SetNext(tmpEntry);
					else
						m_pHeaderEntry = tmpEntry;
					
					tmpEntry->SetNext(pEntry);
					pEntry->SetPrev(tmpEntry);
				}
			}

			virtual void Clear()
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock, 0);

				m_bClearing = 1;

				CLineEntry * pEntry = m_pHeaderEntry;

				m_bHasAllLines = 0;

				while(pEntry != NULL)
				{
					pEntry->NotifyDataChange();
					pEntry->NotifyNextChange();
					pEntry->NotifyPrevChange();

					CLineEntry * pTmp = pEntry;
					pEntry = pEntry->GetNext();

					delete pTmp;
				}

				m_pHeaderEntry = m_pTailEntry = NULL;

				NotifyNewEntryCome();

				m_bClearing = 0;
			}

			virtual void HasAllLines(int getAllLines = 1)
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock, 0);

				m_bHasAllLines = getAllLines;

				CLineEntry * pEntry = m_pHeaderEntry;

				while(pEntry != NULL)
				{
					pEntry->NotifyDataChange();
					pEntry->NotifyNextChange();
					pEntry->NotifyPrevChange();

					pEntry = pEntry->GetNext();
				}

				NotifyNewEntryCome();
			}

			virtual int IsHasAllLines()
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock);
				return m_bHasAllLines;
			}
		};
	}
}
