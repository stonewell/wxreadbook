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
		};
	}
}
