#pragma once

namespace TextProcess
{
	namespace Document
	{
		class IDocumentLine : public virtual ILine
		{
		public:
			virtual void GetData(int nOffset, int nLength, wxChar * pBuf, int * pBuf_Size = NULL) const = 0;
			virtual const wxString & GetData(int nOffset, int nLength) const = 0;
		};
	}
}