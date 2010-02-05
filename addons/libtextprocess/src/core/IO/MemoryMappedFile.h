#pragma once

namespace TextProcess
{
	namespace IO
	{
		class CMemoryMappedFile
		{
		public:
			CMemoryMappedFile(LPCTSTR pszFile);
			virtual ~CMemoryMappedFile(void);

			const CHAR *Buffer() { return m_p; }
			DWORD Length() const { return m_cb; }

		private:
			PCHAR   m_p;
			wxFileOffset   m_cb;
			HANDLE  m_hf;
			HANDLE  m_hfm;
		};
	}
}
