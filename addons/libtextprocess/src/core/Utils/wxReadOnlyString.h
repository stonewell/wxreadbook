#pragma once

namespace TextProcess
{
	namespace Utils
	{
		class wxReadOnlyString : public wxString
		{
        private:
            size_t m_pchDataLength;
		public:
			wxReadOnlyString(wxChar * pBuf, size_t pBufLen)
			{
			    m_pchData = pBuf;
			    m_pchDataLength = pBufLen;

			    GetStringData()->Lock();
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
                    GetStringData()->nDataLength = newSize;
			    }

                return GetStringData()->nDataLength;
			}

			size_t GetPchDataLength() const { return m_pchDataLength; }
		};
	}
}
