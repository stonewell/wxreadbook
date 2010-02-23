#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

inline wxUint32 RoundUp(wxUint32 cb, wxUint32 units)
{
	return ((cb + units - 1) / units) * units;
}

TextProcess::Utils::Impl::CStringPool::CStringPool()
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

TextProcess::Utils::Impl::CStringPool::~CStringPool()
{
	Clear();
}

wxByte * TextProcess::Utils::Impl::CStringPool::AllocBuffer(wxUint32 cch)
{
	{
		TextProcess::Utils::CCriticalSectionAccessor a(&m_Section);

		wxByte * psz = m_pchNext;
		wxUint32 cbAlloc = 0;
		wxByte * pbNext = NULL;
		HEADER* phdrCur = NULL;
		const int wxStringDataSize = sizeof(wxStringData);

		if (m_pchNext + cch <= m_pchLimit)
		{
			m_pchNext += cch;

			return psz;
		}

		if (cch > MAX_CHARALLOC)
		{
			goto OOM;
		}

		cbAlloc = RoundUp(cch + sizeof(HEADER),
			m_dwGranularity);

#ifdef _WIN32
		pbNext = reinterpret_cast<wxByte*>(
			VirtualAlloc(NULL, cbAlloc, MEM_COMMIT, PAGE_READWRITE));
#else
		pbNext = reinterpret_cast<wxByte*>(malloc(cbAlloc * sizeof(wxByte)));
#endif

		if (!pbNext)
		{
OOM:
			static std::bad_alloc OOM;
			throw(OOM);
		}

		m_pchLimit = pbNext + cbAlloc;
		phdrCur = reinterpret_cast<HEADER*>(pbNext);
		phdrCur->m_phdrPrev = m_phdrCur;
		phdrCur->m_cb = cbAlloc;
		m_phdrCur = phdrCur;
		m_pchNext = reinterpret_cast<wxByte*>(phdrCur + 1);
	}

	return AllocBuffer(cch);
}

wxChar * TextProcess::Utils::Impl::CStringPool::AllocString(const wxChar* pszBegin, const wxChar* pszEnd)
{
	size_t cch = pszEnd - pszBegin + 1;

	wxChar * psz = AllocReadOnlyString(cch);
	wxStrncpy(psz, pszBegin, cch);

	return psz;
}

void TextProcess::Utils::Impl::CStringPool::Clear()
{
	TextProcess::Utils::CCriticalSectionAccessor a(&m_Section);

	HEADER* phdr = m_phdrCur;
	while (phdr) 
	{
		HEADER hdr = *phdr;

#ifdef _WIN32
		VirtualFree(phdr, hdr.m_cb, MEM_RELEASE);
#else
		free(phdr);
#endif
		phdr = hdr.m_phdrPrev;
	}

	m_pchNext = NULL;
	m_pchLimit = NULL;
	m_phdrCur = NULL;
}

wxChar * TextProcess::Utils::Impl::CStringPool::AllocReadOnlyString(wxUint32 cch)
{
	const int wxStringDataSize = sizeof(wxStringData);

	wxByte * pBuf = AllocBuffer((cch + 1) * sizeof(wxChar) + wxStringDataSize);
	wxStringData * pStringData = reinterpret_cast<wxStringData *>(pBuf);

	pStringData->nDataLength = cch;
	pStringData->nRefs = 10000;
	pStringData->nAllocLength = cch + 1;
	pStringData->data()[cch] = wxT('\0');

	return pStringData->data();
}
