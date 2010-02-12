#pragma once

namespace TextProcess
{
    namespace Utils
    {
        class ICriticalSection
        {
        public:
			DECLARE_TPL_INTERFACE(ICriticalSection);
			static ICriticalSection * CreateCriticalSection();

            virtual void Enter(void) = 0;
            virtual void Leave(void) = 0;
        };

        class CCriticalSectionAccessor
        {
        private:
            ICriticalSection * m_pSec;
        public:
            CCriticalSectionAccessor(ICriticalSection * pSec) :
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

