#pragma once

namespace TextProcess
{
	namespace Document
	{
		namespace Impl
		{
			class CDocumentLineImpl :
				public virtual TextProcess::Impl::CLineImpl
				, public virtual IDocumentLine
			{
			public:
				CDocumentLineImpl(void);
				virtual ~CDocumentLineImpl(void);

			public:
                IMPL_PROPERTY(TextProcess::IO::IMemoryMappedFile *, DocumentFile);
				virtual void GetData(wxFileOffset nOffset, wxFileOffset nLength, wxChar ** ppBuf, wxFileOffset * ppBufLen);
				virtual wxString GetData(wxFileOffset nOffset, wxFileOffset nLength);

				virtual wxInt32 GetDecodedLength()
				{
					GetDecodedBuffer();
					return m_nDecodedLength;
				}
			private:
				wxChar * GetDecodedBuffer();

				wxChar * m_DecodedBuffer;
				wxInt32 m_nDecodedLength;
				TextProcess::Utils::CCriticalSection m_CriticalSection;
			};
		}
	}
}
