#include "../TextProcess.h"
#include "../Impl/TextProcessImpl.h"

inline wxUint32 RoundUp(wxUint32 cb, wxUint32 units)
{
	return ((cb + units - 1) / units) * units;
}

TextProcess::Utils::CStringPool::CStringPool()
: m_pchNext(NULL), m_pchLimit(NULL), m_phdrCur(NULL)
{
#ifdef _WIN32
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	m_dwGranularity = RoundUp(sizeof(HEADER) + MIN_CBCHUNK,
		si.dwAllocationGranularity);
#else
	m_dwGranularity = RoundUp(sizeof(HEADER) + MIN_CBCHUNK,
		sysconf(_SC_PAGE_SIZE));
#endif
}

TextProcess::Utils::CStringPool::~CStringPool()
{
	HEADER* phdr = m_phdrCur;
	while (phdr) {
		HEADER hdr = *phdr;
		phdr = hdr.m_phdrPrev;

#ifdef _WIN32
		VirtualFree(hdr.m_phdrPrev, hdr.m_cb, MEM_RELEASE);
#else
		free(hdr.m_phdrPrev);
#endif
	}
}

wxChar * TextProcess::Utils::CStringPool::AllocBuffer(wxUint32 cch)
{
	wxChar * psz = m_pchNext;

	if (m_pchNext + cch * sizeof(wxChar) <= m_pchLimit) 
	{
		m_pchNext += (cch * sizeof(wxChar));

		return psz;
	}

	if (cch > MAX_CHARALLOC) goto OOM;
	
	wxUint32 cbAlloc = RoundUp(cch * sizeof(wxChar) + sizeof(HEADER),
		m_dwGranularity);

#ifdef _WIN32
	wxByte* pbNext = reinterpret_cast<wxByte*>(
		VirtualAlloc(NULL, cbAlloc, MEM_COMMIT, PAGE_READWRITE));
#else
	wxByte* pbNext = reinterpret_cast<wxByte*>(malloc(cbAlloc * sizeof(wxByte)));
#endif
	
	if (!pbNext) 
	{
OOM:
		static std::bad_alloc OOM;
		throw(OOM);
	}

	m_pchLimit = reinterpret_cast<wxChar*>(pbNext + cbAlloc);
	HEADER* phdrCur = reinterpret_cast<HEADER*>(pbNext);
	phdrCur->m_phdrPrev = m_phdrCur;
	phdrCur->m_cb = cbAlloc;
	m_phdrCur = phdrCur;
	m_pchNext = reinterpret_cast<wxChar*>(phdrCur + 1);

	return AllocBuffer(cch);
}

wxChar * TextProcess::Utils::CStringPool::AllocString(const wxChar* pszBegin, const wxChar* pszEnd)
{
	size_t cch = pszEnd - pszBegin + 1;

	wxChar * psz = AllocBuffer(cch);
	wxStrncpy(psz, pszBegin, cch);

	return psz;
}
