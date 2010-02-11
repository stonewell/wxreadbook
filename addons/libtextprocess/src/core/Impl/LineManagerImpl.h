#pragma once

namespace TextProcess
{
	namespace Impl
	{
		template<class T, class U>
		class CLineManagerImpl
		{
			class CLineEntry
			{
			public:
				CLineEntry() :
					m_pLine(NULL)
					, m_pPrevEntry(NULL)
					, m_pNextEntry(NULL)
				{
				}

				~CLineEntry()
				{
					TextProcess::Utils::CCriticalSectionAccessor accessor(&m_AccessLock);
					if (m_pLine != NULL) delete m_pLine;
					m_pLine = NULL;
				}
			private:
				T * m_pLine;
				CLineEntry * m_pPrevEntry;
				CLineEntry * m_pNextEntry;

				TextProcess::Utils::CCriticalSection m_AccessLock;

				TextProcess::Utils::CEvent m_hNextWaitEvent;
				TextProcess::Utils::CEvent m_hPrevWaitEvent;
				TextProcess::Utils::CEvent m_hDataWaitEvent;

			public:
				void NotifyNextChange()
				{
					m_hNextWaitEvent.Set();
				}

				void NotifyPrevChange()
				{
					m_hPrevWaitEvent.Set();
				}

				void NotifyDataChange()
				{
					m_hDataWaitEvent.Set();
				}

				T * GetLine()
				{
					TextProcess::Utils::CCriticalSectionAccessor accessor(&m_AccessLock);
					return m_pLine;
				}

				void SetLine(T * pLine)
				{
					TextProcess::Utils::CCriticalSectionAccessor accessor(&m_AccessLock);
					m_pLine = pLine;
					NotifyDataChange();
				}

				void WaitData()
				{
					m_hDataWaitEvent.Wait();
				}

				CLineEntry * GetNext()
				{
				    TextProcess::Utils::CCriticalSectionAccessor accessor(&m_AccessLock);
				    return m_pNextEntry;
                }

				void SetNext(CLineEntry * pNext)
				{
					TextProcess::Utils::CCriticalSectionAccessor accessor(&m_AccessLock);
					m_pNextEntry = pNext;
					NotifyNextChange();
				}

				void WaitNext()
				{
					m_hNextWaitEvent.Wait();
				}

				CLineEntry * GetPrev() 
				{
					TextProcess::Utils::CCriticalSectionAccessor accessor(&m_AccessLock); 
					return m_pPrevEntry; 
				}

				void SetPrev(CLineEntry * pPrev)
				{
					TextProcess::Utils::CCriticalSectionAccessor accessor(&m_AccessLock);
					m_pPrevEntry = pPrev;
					NotifyPrevChange();
				}

				void WaitPrev()
				{
					m_hPrevWaitEvent.Wait();
				}
			};

		private:
			TextProcess::Utils::CReadWriteLock m_ReadWriteLock;

			int m_bHasAllNextLines;
			int m_bHasAllPreviousLines;
			int m_bClearing;

			CLineEntry * m_pHeaderEntry;
			CLineEntry * m_pTailEntry;

			TextProcess::Utils::CMultiWaitEvent m_NewDataEvent;

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
				m_bHasAllPreviousLines(0),
				m_bHasAllNextLines(0),
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

							if (!IsHasAllNextLines() && wait)
								pNextEntry->WaitData();
							else
								return NULL;

							if (m_bClearing)
								return NULL;

							m_ReadWriteLock.LockRead();
						}
					}

					m_ReadWriteLock.UnlockRead();

					if (!IsHasAllNextLines() && wait)
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

							if (!IsHasAllPreviousLines() && wait)
								pPrevEntry->WaitData();
							else
								return NULL;

							if (m_bClearing)
								return NULL;

							m_ReadWriteLock.LockRead();
						}
					}

					m_ReadWriteLock.UnlockRead();

					if (!IsHasAllPreviousLines() && wait)
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

				NotifyNewEntryCome();
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

				NotifyNewEntryCome();
			}

			virtual void Clear()
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock, 0);

				m_bClearing = 1;

				CLineEntry * pEntry = m_pHeaderEntry;

				m_bHasAllPreviousLines = 0;
				m_bHasAllNextLines = 0;

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

			virtual void HasAllPreviousLines()
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock, 0);

				if (m_bHasAllPreviousLines)
					return;

				m_bHasAllPreviousLines = 1;

				CLineEntry * pEntry = m_pHeaderEntry;

				while(pEntry != NULL)
				{
					pEntry->NotifyDataChange();
					pEntry->NotifyPrevChange();

					pEntry = pEntry->GetNext();
				}

				NotifyNewEntryCome();
			}

			virtual void HasAllNextLines()
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock, 0);

				if (m_bHasAllNextLines)
					return;

				m_bHasAllNextLines = 1;

				CLineEntry * pEntry = m_pHeaderEntry;

				while(pEntry != NULL)
				{
					pEntry->NotifyDataChange();
					pEntry->NotifyNextChange();

					pEntry = pEntry->GetNext();
				}

				NotifyNewEntryCome();
			}

			virtual int IsHasAllLines()
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock);
				return m_bHasAllPreviousLines && m_bHasAllNextLines;
			}

			virtual int IsHasAllPreviousLines()
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock);
				return m_bHasAllPreviousLines;
			}

			virtual int IsHasAllNextLines()
			{
				TextProcess::Utils::CReadWriteLocker locker(&m_ReadWriteLock);
				return m_bHasAllNextLines;
			}
		};
	}
}
