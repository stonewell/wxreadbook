#pragma once

namespace TextProcess
{
	namespace Document
	{
		class IDocumentLine : public virtual ILine
		{
		public:
            DECLARE_PROPERTY(TextProcess::IO::IMemoryMappedFile *, DocumentFile);
			virtual void GetData(int nOffset, int nLength, wxChar ** ppBuf, int * ppBufLen) = 0;
			virtual wxString GetData(int nOffset, int nLength) = 0;
			virtual wxInt32 GetDecodedLength() = 0;
		};
	}
}
