#pragma once

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <string>

#if wxUSE_UNICODE
typedef std::basic_string<wchar_t> String;
#else
typedef std::basic_string<char> String;
#endif
