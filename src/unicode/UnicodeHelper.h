#ifndef __UNICODE_HELPER_H__
#define __UNICODE_HELPER_H__

bool IsValidUnicode(wxChar ch);
bool GuessDataEncoding(wxInputStream * pInput, wxString & charsets, int lang);

#endif
