#include "wx/wx.h"
#include "MemoryMappedFile.h"

TextProcess::IO::CMemoryMappedFile::CMemoryMappedFile(const wxChar * pszFile)
: m_hfm(NULL), m_p(NULL), m_cb(0)
{
	m_hf = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hf != INVALID_HANDLE_VALUE) 
	{
		wxFileOffset cb = GetFileSize(m_hf, NULL);
		m_hfm = CreateFileMapping(m_hf, NULL, PAGE_READONLY, 0, 0, NULL);
		if (m_hfm != NULL) 
		{
			m_p = reinterpret_cast<PCHAR>
				(MapViewOfFile(m_hfm, FILE_MAP_READ, 0, 0, cb));

			if (m_p) 
			{
				m_cb = cb;
			}
		}
	}
}

TextProcess::IO::CMemoryMappedFile::~CMemoryMappedFile(void)
{
	if (m_p) UnmapViewOfFile(m_p);
	if (m_hfm) CloseHandle(m_hfm);
	if (m_hf != INVALID_HANDLE_VALUE) CloseHandle(m_hf);
}
