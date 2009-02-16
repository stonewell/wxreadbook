// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/hashmap.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
#endif

#include "wx/datstrm.h"
#include "wx/mstream.h"
#include "wx/ptr_scpd.h"
#include "wx/wfstream.h"

#include "7ZipEntry.h"
#include "7ZipOutputStream.h"
#include "7ZipInputStream.h"
#include "7ZipClassFactory.h"

IMPLEMENT_DYNAMIC_CLASS(C7ZipClassFactory, wxArchiveClassFactory)

/////////////////////////////////////////////////////////////////////////////
// Class factory

C7ZipClassFactory g_7ZipClassFactory;

C7ZipClassFactory::C7ZipClassFactory()
{
    if (this == &g_7ZipClassFactory)
        PushFront();
}

const wxChar * const *
C7ZipClassFactory::GetProtocols(wxStreamProtocolType type) const
{
    static const wxChar *protocols[] = { _T("rar"), NULL };
    static const wxChar *mimetypes[] = { _T("application/rar"), NULL };
    static const wxChar *fileexts[]  = { _T(".rar"), NULL };
    static const wxChar *empty[]     = { NULL };

    switch (type) {
        case wxSTREAM_PROTOCOL: return protocols;
        case wxSTREAM_MIMETYPE: return mimetypes;
        case wxSTREAM_FILEEXT:  return fileexts;
        default:                return empty;
    }
}

