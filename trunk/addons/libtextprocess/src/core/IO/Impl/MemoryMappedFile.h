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
                CMemoryMappedFile(const wxString & strFileName, wxMBConv * pEncoding);
                CMemoryMappedFile(wxInputStream * pInput, wxMBConv * pEncoding);
                virtual ~CMemoryMappedFile(void);

                virtual const wxByte * GetBuffer() { return m_p; }
                virtual wxFileOffset GetLength() const { return m_cb; }

				virtual wxChar * DecodeData(wxFileOffset nOffset, wxFileOffset nLength, wxInt32 & nDecodedSize);
				virtual void Reset();

				IMPL_PROPERTY(wxMBConv *, Encoding);

            private:
                wxByte * m_p;
                wxFileOffset   m_cb;
				TextProcess::Utils::CStringPool m_StringPool;

    #ifdef _WIN32
                HANDLE  m_hf;
                HANDLE  m_hfm;
    #else
                int m_hf;
				int m_do_unlink;

#define SHARE_MEMORY_NAME "/wxReadBook_MemoryFile"

    #endif
            };
	    }
	}
}
