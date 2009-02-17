// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "7ZipEntry.h"

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

IMPLEMENT_DYNAMIC_CLASS(C7ZipEntry, wxArchiveEntry)

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(C7ZipEntryList_)

/////////////////////////////////////////////////////////////////////////////
// ZipEntry

C7ZipEntry::C7ZipEntry(
    const wxString& name /*=wxEmptyString*/,
    const wxDateTime& dt /*=wxDateTime::Now()*/,
    wxFileOffset size    /*=wxInvalidOffset*/)
  :
    m_VersionMadeBy(wxMAJOR_VERSION * 10 + wxMINOR_VERSION),
    m_VersionNeeded(2),
    m_Flags(0),
    m_Method(0),
    m_DateTime(dt),
    m_Crc(0),
    m_CompressedSize(wxInvalidOffset),
    m_Size(size),
    m_Key(wxInvalidOffset),
    m_Offset(wxInvalidOffset),
    m_DiskStart(0),
    m_InternalAttributes(0),
    m_ExternalAttributes(0),
	m_bIsDir(false)
{
    if (!name.empty())
        SetName(name);
}

C7ZipEntry::~C7ZipEntry()
{
}

C7ZipEntry::C7ZipEntry(const C7ZipEntry& e)
  : wxArchiveEntry(e),
    m_SystemMadeBy(e.m_SystemMadeBy),
    m_VersionMadeBy(e.m_VersionMadeBy),
    m_VersionNeeded(e.m_VersionNeeded),
    m_Flags(e.m_Flags),
    m_Method(e.m_Method),
    m_DateTime(e.m_DateTime),
    m_Crc(e.m_Crc),
    m_CompressedSize(e.m_CompressedSize),
    m_Size(e.m_Size),
    m_Name(e.m_Name),
    m_Key(e.m_Key),
    m_Offset(e.m_Offset),
    m_Comment(e.m_Comment),
    m_DiskStart(e.m_DiskStart),
    m_InternalAttributes(e.m_InternalAttributes),
    m_ExternalAttributes(e.m_ExternalAttributes),
	m_bIsDir(e.m_bIsDir),
	m_nArchiveIndex(e.m_nArchiveIndex)
{
}

C7ZipEntry& C7ZipEntry::operator=(const C7ZipEntry& e)
{
    if (&e != this) {
        m_SystemMadeBy = e.m_SystemMadeBy;
        m_VersionMadeBy = e.m_VersionMadeBy;
        m_VersionNeeded = e.m_VersionNeeded;
        m_Flags = e.m_Flags;
        m_Method = e.m_Method;
        m_DateTime = e.m_DateTime;
        m_Crc = e.m_Crc;
        m_CompressedSize = e.m_CompressedSize;
        m_Size = e.m_Size;
        m_Name = e.m_Name;
        m_Key = e.m_Key;
        m_Offset = e.m_Offset;
        m_Comment = e.m_Comment;
        m_DiskStart = e.m_DiskStart;
        m_InternalAttributes = e.m_InternalAttributes;
        m_ExternalAttributes = e.m_ExternalAttributes;
		m_bIsDir = e.m_bIsDir;
		m_nArchiveIndex = e.m_nArchiveIndex;
    }
    return *this;
}

wxString C7ZipEntry::GetName(wxPathFormat format /*=wxPATH_NATIVE*/) const
{
    bool isDir = IsDir() && !m_Name.empty();

    // optimisations for common (and easy) cases
    switch (wxFileName::GetFormat(format)) {
        case wxPATH_DOS:
        {
            wxString name(isDir ? m_Name + _T("\\") : m_Name);
            for (size_t i = 0; i < name.length(); i++)
                if (name[i] == _T('/'))
                    name[i] = _T('\\');
            return name;
        }

        case wxPATH_UNIX:
            return isDir ? m_Name + _T("/") : m_Name;

        default:
            ;
    }

    wxFileName fn;

    if (isDir)
        fn.AssignDir(m_Name, wxPATH_UNIX);
    else
        fn.Assign(m_Name, wxPATH_UNIX);

    return fn.GetFullPath(format);
}

// Static - Internally tars and zips use forward slashes for the path
// separator, absolute paths aren't allowed, and directory names have a
// trailing slash.  This function converts a path into this internal format,
// but without a trailing slash for a directory.
//
wxString C7ZipEntry::GetInternalName(const wxString& name,
                                     wxPathFormat format /*=wxPATH_NATIVE*/,
                                     bool *pIsDir        /*=NULL*/)
{
    wxString internal;

    if (wxFileName::GetFormat(format) != wxPATH_UNIX)
        internal = wxFileName(name, format).GetFullPath(wxPATH_UNIX);
    else
        internal = name;

    bool isDir = !internal.empty() && internal.Last() == '/';
    if (pIsDir)
        *pIsDir = isDir;
    if (isDir)
        internal.erase(internal.length() - 1);

    while (!internal.empty() && *internal.begin() == '/')
        internal.erase(0, 1);
    while (!internal.empty() && internal.compare(0, 2, _T("./")) == 0)
        internal.erase(0, 2);
    if (internal == _T(".") || internal == _T(".."))
        internal = wxEmptyString;

    return internal;
}

void C7ZipEntry::SetIsDir(bool isDir /*=true*/)
{
	m_bIsDir = isDir;
}

// Return unix style permission bits
//
int C7ZipEntry::GetMode() const
{
    // return unix permissions if present
    if (IsMadeByUnix())
        return (m_ExternalAttributes >> 16) & 0777;

    // otherwise synthesize from the dos attribs
    int mode = 0644;

    return mode;
}

// Set unix permissions
//
void C7ZipEntry::SetMode(int mode)
{
}

