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
				virtual wxByte * AllocBuffer(wxUint32 cch);
				virtual wxChar * AllocReadOnlyString(wxUint32 cch);
				virtual void Clear();

			private:
				union HEADER {
					struct {
						HEADER* m_phdrPrev;
						wxFileOffset  m_cb;
					};
					wxByte alignment;
				};

				enum 
				{ 
					MIN_CBCHUNK = 64 * 1024,
					MAX_CHARALLOC = 1024 * 1024 * 1024 
				};

			private:
				wxByte *  m_pchNext;   // first available byte
				wxByte *  m_pchLimit;  // one past last available byte
				HEADER*  m_phdrCur;   // current block
				wxUint32   m_dwGranularity;

				CCriticalSection m_Section;
			};
		}
	}
}
