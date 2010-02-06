#pragma once

namespace TextProcess
{
    namespace Utils
    {
        /**
         * @class A wrapper-class around Critical Section functionality, WIN32 & PTHREADS.
         */
        class CCriticalSection
        {
        public:
            /**
             * @brief CriticalSection class constructor.
             */
            CCriticalSection(void)
            {
            #ifdef _WIN32
                if (0 == InitializeCriticalSectionAndSpinCount(&this->m_cSection, 0))
                    throw("Could not create a CriticalSection");
            #else
                if (pthread_mutex_init(&this->m_cSection, NULL) != 0)
                    throw("Could not create a CriticalSection");
            #endif
            }; // CriticalSection()

            /**
             * @brief CriticalSection class destructor
             */
            ~CCriticalSection(void)
            {
                this->WaitForFinish(); // acquire ownership
            #ifdef _WIN32
                DeleteCriticalSection(&this->m_cSection);
            #else
                pthread_mutex_destroy(&this->m_cSection);
            #endif
            }; // ~CriticalSection()

            /**
             * @fn void WaitForFinish(void)
             * @brief Waits for the critical section to unlock.
             * This function puts the waiting thread in a waiting
             * state.
             * @see TryEnter()
             * @return void
             */
            void WaitForFinish(void)
            {
                while(!this->TryEnter())
                {
                #ifdef _WIN32
                    Sleep(1); // put waiting thread to sleep for 1ms
                #else
                    usleep(1000); // put waiting thread to sleep for 1ms (1000us)
                #endif
                };
            }; // WaitForFinish()

            /**
             * @fn void Enter(void)
             * @brief Wait for unlock and enter the CriticalSection object.
             * @see TryEnter()
             * @return void
             */
            void Enter(void)
            {
                this->WaitForFinish(); // acquire ownership
            #ifdef _WIN32
                EnterCriticalSection(&this->m_cSection);
            #else
                pthread_mutex_lock(&this->m_cSection);
            #endif
            }; // Enter()

            /**
             * @fn void Leave(void)
             * @brief Leaves the critical section object.
             * This function will only work if the current thread
             * holds the current lock on the CriticalSection object
             * called by Enter()
             * @see Enter()
             * @return void
             */
            void Leave(void)
            {
            #ifdef _WIN32
                LeaveCriticalSection(&this->m_cSection);
            #else
                pthread_mutex_unlock(&this->m_cSection);
            #endif
            }; // Leave()

            /**
             * @fn bool TryEnter(void)
             * @brief Attempt to enter the CriticalSection object
             * @return bool(true) on success, bool(false) if otherwise
             */
            bool TryEnter(void)
            {
                // Attempt to acquire ownership:
            #ifdef _WIN32
                return(TRUE == TryEnterCriticalSection(&this->m_cSection));
            #else
                return(0 == pthread_mutex_trylock(&this->m_cSection));
            #endif
            }; // TryEnter()

        private:
        #ifdef _WIN32
            CRITICAL_SECTION m_cSection; //!< internal system critical section object (windows)
        #else
            pthread_mutex_t m_cSection; //!< internal system critical section object (*nix)
        #endif
        }; // class CriticalSection

        class CCriticalSectionAccessor
        {
        private:
            CCriticalSection * m_pSec;
        public:
            CCriticalSectionAccessor(CCriticalSection * pSec) :
              m_pSec(pSec)
              {
                  m_pSec->Enter();
              }

              ~CCriticalSectionAccessor()
              {
                  m_pSec->Leave();
              }
        };
    }
}
