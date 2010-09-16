#pragma once

#if wxMAJOR_VERSION <= 2 && wxMINOR_VERSION < 9
namespace TextProcess {
	namespace Utils {
		namespace Impl {
			class wxReadOnlyString: public wxString {
			private:
				size_t m_pchDataLength;
				wxChar m_SavedChar;
			public:
				wxReadOnlyString(wxChar * pBuf, size_t pBufLen) {
					m_pchData = pBuf;
					m_pchDataLength = pBufLen;

					GetStringData()->Lock();
					m_SavedChar = m_pchData[length()];
				}

				~wxReadOnlyString() {
					m_pchData[length()] = m_SavedChar;
				}

				wxReadOnlyString(const wxString & string) :
					wxString(string) {
					m_pchDataLength = GetStringData()->nDataLength;
				}

				size_t ReadOnlyResize(size_t newSize) {
					if (newSize >= 0 && newSize <= m_pchDataLength) {
						m_pchData[length()] = m_SavedChar;
						GetStringData()->nDataLength = newSize;
						m_SavedChar = m_pchData[length()];
						m_pchData[length()] = wxT('\0');
					}

					return GetStringData()->nDataLength;
				}

				size_t GetPchDataLength() const {
					return m_pchDataLength;
				}
			};
		}
	}
}

#endif

