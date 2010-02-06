#pragma once

namespace TextProcess
{
	namespace Document
	{
		class IDocumentLine : public virtual ILine
		{
		public:
            DECLARE_PROPERTY(TextProcess::IO::IMemoryMappedFile *, DocumentFile);
			virtual void GetData(int nOffset, int nLength, wxChar * pBuf, int * pBuf_Size = NULL) const = 0;
			virtual const wxString & GetData(int nOffset, int nLength) const = 0;
		};
	}
}
