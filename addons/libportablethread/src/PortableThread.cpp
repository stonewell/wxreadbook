#include "../includes/PortableThread.h"

#ifdef _WIN32
#include <Windows.h>

namespace PortableThread
{
	class CCS
	{
	public:
		CCS(CRITICAL_SECTION  * pSection) : m_pSection(pSection)
		{
			::EnterCriticalSection(m_pSection);
		}
		
		~CCS()
		{
			::LeaveCriticalSection(m_pSection);
		}

		CRITICAL_SECTION * m_pSection;
	};

	class CPortableThreadHandle
	{
	public:
		friend class CPortableThread;
		HANDLE m_hThreadHandle;
		DWORD m_dwThreadId;
		CRITICAL_SECTION m_Section;

		CPortableThreadHandle()
		{
			m_hThreadHandle = INVALID_HANDLE_VALUE;
			m_dwThreadId = 0;
			InitializeCriticalSection(&m_Section);
		}

		~CPortableThreadHandle()
		{
			DeleteCriticalSection(&m_Section);
		}
	private:
		static unsigned long __stdcall PortableThreadWin32Func( void * lpParam )
		{
			CPortableThread * pThread = reinterpret_cast<CPortableThread *>(lpParam);

			pThread->DoThreadWork();

			return pThread->GetThreadExitCode();
		}
	};
}
#else
#endif

PortableThread::CPortableThread::CPortableThread() :
m_pRunnable(NULL)
, m_nThreadExitCode(0)
, m_pRunningArgument(NULL)
{
	m_pThreadHandle = new PortableThread::CPortableThreadHandle();
}

PortableThread::CPortableThread::CPortableThread(IPortableRunnable * pRunnable) :
m_pRunnable(pRunnable)
, m_nThreadExitCode(0)
, m_pRunningArgument(NULL)
{
	m_pThreadHandle = new PortableThread::CPortableThreadHandle();
}

PortableThread::CPortableThread::~CPortableThread()
{
	Abort();

	delete m_pThreadHandle;
}

void PortableThread::CPortableThread::Start()
{
#ifdef _WIN32
	CCS c(&m_pThreadHandle->m_Section);

	if (m_pThreadHandle->m_hThreadHandle != INVALID_HANDLE_VALUE)
		return;

	m_pThreadHandle->m_hThreadHandle = CreateThread( 
		NULL,              // default security attributes
		0,                 // use default stack size  
		PortableThread::CPortableThreadHandle::PortableThreadWin32Func,          // thread function 
		this,             // argument to thread function 
		0,                 // use default creation flags 
		&m_pThreadHandle->m_dwThreadId);   // returns the thread identifier 

#else
#endif
}

void PortableThread::CPortableThread::Abort()
{
#ifdef _WIN32
	CCS c(&m_pThreadHandle->m_Section);

	if (m_pThreadHandle->m_hThreadHandle != INVALID_HANDLE_VALUE)
	{
		::TerminateThread(m_pThreadHandle->m_hThreadHandle, 0);
		::WaitForSingleObject(m_pThreadHandle->m_hThreadHandle, INFINITE);
		::CloseHandle(m_pThreadHandle->m_hThreadHandle);
		m_pThreadHandle->m_hThreadHandle = INVALID_HANDLE_VALUE;
	}
#endif
}

void PortableThread::CPortableThread::DoThreadWork()
{
	if (m_pRunnable)
		m_nThreadExitCode = m_pRunnable->Run(m_pRunningArgument);
}

bool PortableThread::CPortableThread::Join(int nTimeout)
{
#ifdef _WIN32
	CCS c(&m_pThreadHandle->m_Section);

	if (m_pThreadHandle->m_hThreadHandle != INVALID_HANDLE_VALUE)
	{
		if (WAIT_TIMEOUT == ::WaitForSingleObject(m_pThreadHandle->m_hThreadHandle, nTimeout))
			return false;

		::CloseHandle(m_pThreadHandle->m_hThreadHandle);

		m_pThreadHandle->m_hThreadHandle = INVALID_HANDLE_VALUE;
	}
#endif
	return true;
}
