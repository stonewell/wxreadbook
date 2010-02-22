#pragma once

namespace TextProcess
{
	namespace Utils
	{
		namespace Impl
		{
			class CStringPool
			{
			public:
				CStringPool();
				~CStringPool();

				virtual wxChar * AllocString(const wxChar* pszBegin, const wxChar* pszEnd);
				virtual wxChar * AllocBuffer(wxUint32 cch);
				virtual void Clear();

			private:
				union HEADER {
					struct {
						HEADER* m_phdrPrev;
						wxFileOffset  m_cb;
					};
					wxChar alignment;
				};

				enum 
				{ 
					MIN_CBCHUNK = 64 * 1024,
					MAX_CHARALLOC = 1024 * 1024 * 1024 
				};

			private:
				wxChar*  m_pchNext;   // first available byte
				wxChar*  m_pchLimit;  // one past last available byte
				HEADER*  m_phdrCur;   // current block
				wxUint32   m_dwGranularity;

				CCriticalSection m_Section;
			};
		}
	}
}
