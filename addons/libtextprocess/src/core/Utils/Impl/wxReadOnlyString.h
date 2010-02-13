#pragma once

namespace TextProcess
{
	namespace Utils
	{
		namespace Impl
		{
		class wxReadOnlyString : public wxString
		{
        private:
            size_t m_pchDataLength;
			wxChar m_SavedChar;
		public:
			wxReadOnlyString(wxChar * pBuf, size_t pBufLen)
			{
			    m_pchData = pBuf;
			    m_pchDataLength = pBufLen;

			    GetStringData()->Lock();
				m_SavedChar = GetStringData()->data()[length()];
			}
		
			~wxReadOnlyString()
			{
				GetStringData()->data()[length()] = m_SavedChar;
			}

			wxReadOnlyString(const wxString & string) :
			wxString(string)
			{
			    m_pchDataLength = GetStringData()->nDataLength;
			}

			size_t ReadOnlyResize(size_t newSize)
			{
			    if (newSize >= 0 && newSize <= m_pchDataLength)
			    {
					GetStringData()->data()[length()] = m_SavedChar;
                    GetStringData()->nDataLength = newSize;
					m_SavedChar = GetStringData()->data()[length()];
			    }

                return GetStringData()->nDataLength;
			}

			size_t GetPchDataLength() const { return m_pchDataLength; }
		};
		}
	}
}
