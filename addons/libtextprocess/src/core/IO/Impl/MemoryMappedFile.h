#pragma once

namespace TextProcess
{
	namespace IO
	{
	    namespace Impl
	    {
            class CMemoryMappedFile :
                public virtual IMemoryMappedFile
            {
            public:
                CMemoryMappedFile(const wxString & strFileName);
                virtual ~CMemoryMappedFile(void);

                virtual const wxByte * GetBuffer() { return m_p; }
                virtual wxFileOffset GetLength() const { return m_cb; }

            private:
                wxByte * m_p;
                wxFileOffset   m_cb;
    #ifdef _WIN32
                HANDLE  m_hf;
                HANDLE  m_hfm;
    #else
                int m_hf;
    #endif
            };
	    }
	}
}
