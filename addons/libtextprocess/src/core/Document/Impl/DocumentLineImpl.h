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
				virtual void GetData(int nOffset, int nLength, wxChar * pBuf, int * pBuf_Size = NULL) const;
				virtual const wxString & GetData(int nOffset, int nLength) const;
			};
		}
	}
}
