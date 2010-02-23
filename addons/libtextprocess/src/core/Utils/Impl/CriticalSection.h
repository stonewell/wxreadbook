#pragma once

namespace TextProcess
{
    namespace Utils
    {
		namespace Impl
		{
        /**
         * @class A wrapper-class around Critical Section functionality, WIN32 & PTHREADS.
         */
        class CCriticalSection :
			public ICriticalSection
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
			#ifdef BUILD_ANGSTROM
					exit(2);
			#else
                    throw("Could not create a CriticalSection");
			#endif
            #endif
            }; // CriticalSection()

            /**
             * @brief CriticalSection class destructor
             */
            virtual ~CCriticalSection(void)
            {
            #ifdef _WIN32
                DeleteCriticalSection(&this->m_cSection);
            #else
                pthread_mutex_destroy(&this->m_cSection);
            #endif
            }; // ~CriticalSection()

            /**
             * @fn void Enter(void)
             * @brief Wait for unlock and enter the CriticalSection object.
             * @see TryEnter()
             * @return void
             */
            virtual void Enter(void)
            {
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
            virtual void Leave(void)
            {
            #ifdef _WIN32
                LeaveCriticalSection(&this->m_cSection);
            #else
                pthread_mutex_unlock(&this->m_cSection);
            #endif
            }; // Leave()

        private:
        #ifdef _WIN32
            CRITICAL_SECTION m_cSection; //!< internal system critical section object (windows)
        #else
            pthread_mutex_t m_cSection; //!< internal system critical section object (*nix)
        #endif
        }; // class CriticalSection
		}
   }
}

