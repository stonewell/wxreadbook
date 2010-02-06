#pragma once

#ifndef _WIN32
#define WAIT_OBJECT_0 (0)
#define WAIT_TIMEOUT (1)
#define WAIT_ABANDONED (2)
#define INFINITE (-1)
#endif

namespace TextProcess
{
    namespace Utils
    {
        class CEvent
        {
        public:
            CEvent(int bManualReset = 1, int bInitialized = 0)
            {
            #ifdef _WIN32
                m_hEvent = CreateEvent(NULL,
                    bManualReset ? TRUE : FALSE,
                    bInitialized ? TRUE : FALSE, NULL);
            #else
                m_bManualReset = bManualReset;
                m_bSignaled = bInitialized;
                pthread_mutex_init(&m_Mutex, NULL);
                pthread_cond_init(&m_Cond, NULL);
            #endif
            }

            ~CEvent()
            {
            #ifdef _WIN32
                CloseHandle(m_hEvent);
            #else
                pthread_cond_destroy(&m_Cond);
                pthread_mutex_destroy(&m_Mutex);
            #endif
            }

            void Set()
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

            void Reset()
            {
            #ifdef _WIN32
                ResetEvent(m_hEvent);
            #else
                pthread_mutex_lock(&m_Mutex);
                m_bSignaled = 0;
                pthread_mutex_unlock(&m_Mutex);
            #endif
            }

            int Wait(int nTimeout = -1)
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
                        nTimeout /= 1000;
                        timeout.tv_sec = now.tv_sec + nTimeout;
                        timeout.tv_nsec = now.tv_usec * 1000;
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

            ~CMultiWaitEvent()
            {
            }

            void NotifyAll()
            {
                CCriticalSectionAccessor a(&m_AccessLock);
                m_Event.Set();
            }

            void Wait()
            {
                {
                    CCriticalSectionAccessor a(&m_AccessLock);

                    m_nWait++;
                }

                m_Event.Wait();

                {
                    CCriticalSectionAccessor a(&m_AccessLock);
                    m_nWait--;

                    if (!m_nWait)
                        m_Event.Reset();
                }
            }
        };
    }
}

