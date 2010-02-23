#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::IO::Impl::CMemoryMappedFile::CMemoryMappedFile(const wxString & strFileName, wxMBConv * pEncoding)
: m_p(NULL), m_cb(0),
INIT_PROPERTY(Encoding, pEncoding)
#ifdef _WIN32
, m_hfm(NULL), m_hf(NULL)
#else
, m_hf(-1)
, m_do_unlink(0)
#endif
{
#ifdef _WIN32
	m_hf = CreateFile(strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hf != INVALID_HANDLE_VALUE)
	{
		wxFileOffset cb = GetFileSize(m_hf, NULL);
		m_hfm = CreateFileMapping(m_hf, NULL, PAGE_READONLY, 0, 0, NULL);
		if (m_hfm != NULL)
		{
			m_p = reinterpret_cast<wxByte *>
				(MapViewOfFile(m_hfm, FILE_MAP_READ, 0, 0, cb));

			if (m_p)
			{
				m_cb = cb;
			}
		}
	}
#else
	struct stat sb;
	off_t pa_offset;

#if wxUSE_UNICODE
	m_hf = open(strFileName.ToUTF8(), O_RDONLY);
#else
	m_hf = open(strFileName.c_str(), O_RDONLY);
#endif

	if (m_hf == -1) return;

	if (fstat(m_hf, &sb) == -1) return;

	pa_offset = 0 & ~(sysconf(_SC_PAGE_SIZE) - 1);

	m_cb = sb.st_size;

	m_p = reinterpret_cast<wxByte *>(mmap(NULL, m_cb + 0 - pa_offset, PROT_READ,
		MAP_PRIVATE, m_hf, 0));

	if (m_p == MAP_FAILED)
	{
		m_p = NULL;
		return;
	}

#endif
}

TextProcess::IO::Impl::CMemoryMappedFile::CMemoryMappedFile(wxInputStream * pInput, wxMBConv * pEncoding)
: m_p(NULL), m_cb(0),
INIT_PROPERTY(Encoding, pEncoding)
#ifdef _WIN32
, m_hfm(NULL), m_hf(INVALID_HANDLE_VALUE)
#else
, m_hf(-1)
, m_do_unlink(1)
#endif
{
#ifdef _WIN32
	wxFileOffset cb = pInput->GetLength();
	DWORD dwHigh = (cb >> 32) & 0xFFFFFFFF;
	DWORD dwLow = (DWORD)(cb & 0xFFFFFFFF);

	m_hfm = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, 
		PAGE_READWRITE, dwHigh, dwLow, NULL);

	if (m_hfm != NULL)
	{
		m_p = reinterpret_cast<wxByte *>
			(MapViewOfFile(m_hfm, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0));

		if (m_p)
		{
			pInput->SeekI(0);
			pInput->Read(m_p, cb);

			m_cb = cb;
		}
	}
#else
	off_t pa_offset;

TPL_WPRINTF(wxT("shm open \n"));
	m_hf = shm_open(SHARE_MEMORY_NAME, O_RDWR | O_CREAT, 0x1ED);

	if (m_hf == -1) 
	{
		TPL_WPRINTF(wxT("shm open fail, errno=%d, %d, %d, %d,%d,%d,%d,%d\n"), errno,
		EACCES, EEXIST, EINVAL, EMFILE, ENAMETOOLONG, ENFILE, ENOENT);
		return;
	}
TPL_WPRINTF(wxT("shm open success\n"));

	pa_offset = 0 & ~(sysconf(_SC_PAGE_SIZE) - 1);

TPL_WPRINTF(wxT("shm open %ld\n"), pa_offset);
	m_cb = pInput->GetLength();

	ftruncate(m_hf, m_cb);

	m_p = reinterpret_cast<wxByte *>(mmap(NULL, m_cb + 0 - pa_offset, PROT_READ | PROT_WRITE,
		MAP_PRIVATE, m_hf, 0));

TPL_WPRINTF(wxT("mmap %d %d\n"), pa_offset, m_cb);
	if (m_p == MAP_FAILED)
	{
		m_p = NULL;
		return;
	}

TPL_WPRINTF(wxT("mmap success %d %d\n"), pa_offset, m_cb);
	pInput->SeekI(0);
	pInput->Read(m_p, m_cb);
TPL_WPRINTF(wxT("Read Done %d %d\n"), pa_offset, m_cb);
#endif
}

TextProcess::IO::Impl::CMemoryMappedFile::~CMemoryMappedFile(void)
{
#ifdef _WIN32
	if (m_p) UnmapViewOfFile(m_p);
	if (m_hfm) CloseHandle(m_hfm);
	if (m_hf != INVALID_HANDLE_VALUE) CloseHandle(m_hf);
#else
	if (m_p) munmap(m_p, m_cb);
	if (m_hf != -1) 
	{
		if (m_do_unlink)
			shm_unlink(SHARE_MEMORY_NAME);
		else
			close(m_hf);
	}
#endif
}

wxChar * TextProcess::IO::Impl::CMemoryMappedFile::DecodeData(wxFileOffset nOffset, wxFileOffset nLength, wxInt32 & nDecodedSize)
{
	const char * pBegin = reinterpret_cast<const char *>(GetBuffer() + nOffset);

	nDecodedSize = GetEncoding()->ToWChar(NULL, 0, pBegin, nLength);

	if (nDecodedSize == wxCONV_FAILED)
	{
		static std::bad_alloc OOM;
		throw(OOM);
	}

	wxChar * pBuf = m_StringPool.AllocReadOnlyString(nDecodedSize);

	GetEncoding()->ToWChar(pBuf, nDecodedSize, pBegin, nLength);

	nDecodedSize--;

	wxChar ch = pBuf[nDecodedSize - 1];

	return pBuf;
}

void TextProcess::IO::Impl::CMemoryMappedFile::Reset()
{
	m_StringPool.Clear();
}
