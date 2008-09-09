#ifndef __VERSION_H__
#define __VERSION_H__

#include <wx/version.h>

#define MAJOR_VERSION      1
#define MINOR_VERSION      0
#define RELEASE_NUMBER     5
#define SUBRELEASE_NUMBER  0
#define VERSION_STRING   _T("wxReadBook 1.0.5")

#define VERSION_NUM_STRING_T \
  wxMAKE_VERSION_STRING_T(MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER)
#define VERSION_NUM_DOT_STRING_T \
  wxMAKE_VERSION_DOT_STRING_T(MAJOR_VERSION, MINOR_VERSION, RELEASE_NUMBER)
#endif
