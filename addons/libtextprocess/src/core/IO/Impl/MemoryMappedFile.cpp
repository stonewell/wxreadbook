#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::IO::Impl::CMemoryMappedFile::CMemoryMappedFile(const wxString & strFileName, wxMBConv * pEncoding)
: m_p(NULL), m_cb(0),
INIT_PROPERTY(Encoding, pEncoding)
#ifdef _WIN32
, m_hfm(NULL), m_hf(NULL)
#else
, m_hf(-1)
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

TextProcess::IO::Impl::CMemoryMappedFile::~CMemoryMappedFile(void)
{
#ifdef _WIN32
	if (m_p) UnmapViewOfFile(m_p);
	if (m_hfm) CloseHandle(m_hfm);
	if (m_hf != INVALID_HANDLE_VALUE) CloseHandle(m_hf);
#else
	if (m_p) munmap(m_p, m_cb);
	if (m_hf != -1) close(m_hf);
#endif
}

wxChar * TextProcess::IO::Impl::CMemoryMappedFile::DecodeData(int nOffset, int nLength)
{
	const char * pBegin = reinterpret_cast<const char *>(GetBuffer() + nOffset);

	size_t dst_size = GetEncoding()->ToWChar(NULL, 0, pBegin, nLength);

	if (dst_size == wxCONV_FAILED)
	{
		static std::bad_alloc OOM;
		throw(OOM);
	}

	wxChar * pBuf = m_StringPool.AllocBuffer(sizeof(wxChar) * (dst_size + 1));

	GetEncoding()->ToWChar(pBuf, dst_size, pBegin, nLength);

	return pBuf;
}