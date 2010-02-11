#pragma once

namespace TextProcess
{
	namespace Document
	{
		class IDocumentLine : public virtual ILine
		{
		public:
			DECLARE_TPL_INTERFACE(IDocumentLine);

			DECLARE_PROPERTY(TextProcess::IO::IMemoryMappedFile *, DocumentFile);
			virtual void GetData(wxFileOffset nOffset, wxFileOffset nLength, wxChar ** ppBuf, wxFileOffset * ppBufLen) = 0;
			virtual wxString GetData(wxFileOffset nOffset, wxFileOffset nLength) = 0;
			virtual wxInt32 GetDecodedLength() = 0;
		};
	}
}
