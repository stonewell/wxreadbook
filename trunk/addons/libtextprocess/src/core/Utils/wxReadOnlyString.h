#pragma once

namespace TextProcess
{
	namespace Utils
	{
		class wxReadOnlyString : public wxString
		{
		public:
			wxReadOnlyString(const wxChar *psz, size_t nLength)
			{ 
				m_pchData = psz;
			}

			virtual ~wxReadOnlyString()
			{
				m_pchData = (wxChar *)wxEmptyString;
			}
		};
	}
}
