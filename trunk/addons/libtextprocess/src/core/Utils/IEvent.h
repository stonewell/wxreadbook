#pragma once

namespace TextProcess
{
    namespace Utils
    {
        class IEvent
        {
        public:
			DECLARE_TPL_INTERFACE(IEvent);

            static IEvent * CreateEvent(int bManualReset = 1, int bInitialized = 0);

            virtual void Set() = 0;
            
            virtual void Reset() = 0;
            
            virtual int Wait(int nTimeout = -1) = 0;
        };

        class IMultiWaitEvent //Auto Reset wait ref count == 0
        {
        public:
			DECLARE_TPL_INTERFACE(IMultiWaitEvent);

            static IMultiWaitEvent * CreateMultiWaitEvent();

            virtual void NotifyAll() = 0;
            
            virtual void Wait() = 0;
        };
    }
}

