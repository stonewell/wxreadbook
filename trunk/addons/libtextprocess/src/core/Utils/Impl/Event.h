#pragma once

namespace TextProcess
{
    namespace Utils
    {
		namespace Impl
		{
        class CEvent :
			public IEvent
        {
        public:
            CEvent(int bManualReset = 1, int bInitialized = 0)
            {
            #ifdef _WIN32
				m_hEvent = ::CreateEvent(NULL,
                    bManualReset ? TRUE : FALSE,
                    bInitialized ? TRUE : FALSE, NULL);
            #else
                m_bManualReset = bManualReset;
                m_bSignaled = bInitialized;
                pthread_mutex_init(&m_Mutex, NULL);
                pthread_cond_init(&m_Cond, NULL);
            #endif
            }

            virtual ~CEvent()
            {
            #ifdef _WIN32
                CloseHandle(m_hEvent);
            #else
                pthread_cond_destroy(&m_Cond);
                pthread_mutex_destroy(&m_Mutex);
            #endif
            }

            virtual void Set()
            {
            #ifdef _WIN32
                SetEvent(m_hEvent);
            #else
                pthread_mutex_lock(&m_Mutex);
                m_bSignaled = 1;
                pthread_cond_broadcast(&m_Cond);
                pthread_mutex_unlock(&m_Mutex);
            #endif
            }

            virtual void Reset()
            {
            #ifdef _WIN32
                ResetEvent(m_hEvent);
            #else
                pthread_mutex_lock(&m_Mutex);
                m_bSignaled = 0;
                pthread_mutex_unlock(&m_Mutex);
            #endif
            }

            virtual int Wait(int nTimeout = -1)
            {
            #ifdef _WIN32
                return WaitForSingleObject(m_hEvent, nTimeout);
            #else
                int result = WAIT_OBJECT_0;
                int retcode = 0;

                pthread_mutex_lock(&m_Mutex);

                if (!m_bSignaled)
                {
                    if (nTimeout != -1)
                    {
                        struct timeval now;
                        struct timespec timeout;
                        struct timezone tz;

                        gettimeofday(&now, &tz);
						if (nTimeout < 1000)
							nTimeout = 1000;
                        timeout.tv_sec = now.tv_sec + nTimeout / 1000;
                        timeout.tv_nsec = (now.tv_usec + (nTimeout % 1000) * 1000) * 1000;
                        retcode = pthread_cond_timedwait(&m_Cond, &m_Mutex, &timeout);
                        if (retcode == ETIMEDOUT) {
                            /* timeout occurred */
                            result = WAIT_TIMEOUT;
                        } else if (retcode != 0) {
                            result = WAIT_ABANDONED;
                        }
                    }
                    else
                    {
                        retcode = pthread_cond_wait(&m_Cond, &m_Mutex);

                        if (retcode != 0) {
                            result = WAIT_ABANDONED;
                        }
                    }
                }

                if (!m_bManualReset && m_bSignaled)
                {
                    m_bSignaled = 0;
                }

                pthread_mutex_unlock(&m_Mutex);

                return result;
            #endif
            }

        private:
        #ifdef _WIN32
            HANDLE m_hEvent;
        #else
            int m_bSignaled;
            int m_bManualReset;
            pthread_cond_t m_Cond;
            pthread_mutex_t m_Mutex;
        #endif

        };

        class CMultiWaitEvent //Auto Reset wait ref count == 0
			: public IMultiWaitEvent
        {
        private:
            int m_nWait;
            CEvent m_Event;
            CCriticalSection m_AccessLock;

        public:
            CMultiWaitEvent() :
              m_nWait(0)
            {
            }

            virtual ~CMultiWaitEvent()
            {
            }

            virtual void NotifyAll()
            {
                CCriticalSectionAccessor a(&m_AccessLock);
                m_Event.Set();
            }

            virtual void Wait()
            {
				m_AccessLock.Enter();
                m_nWait++;
				m_AccessLock.Leave();

                m_Event.Wait();

				m_AccessLock.Enter();
                m_nWait--;

                if (!m_nWait)
                    m_Event.Reset();
				m_AccessLock.Leave();
            }
        };
		}
    }
}

