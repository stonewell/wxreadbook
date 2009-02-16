#ifndef __7ZIP_ENTRY_H__
#define __7ZIP_ENTRY_H__

#include "wx/defs.h"

#include "wx/archive.h"
#include "wx/filename.h"
/////////////////////////////////////////////////////////////////////////////
// Zip Entry - holds the meta data for a file in the zip

class WXDLLIMPEXP_BASE C7ZipEntry : public wxArchiveEntry
{
public:
    C7ZipEntry(const wxString& name = wxEmptyString,
               const wxDateTime& dt = wxDateTime::Now(),
               wxFileOffset size = wxInvalidOffset);
    virtual ~C7ZipEntry();

    C7ZipEntry(const C7ZipEntry& entry);
    C7ZipEntry& operator=(const C7ZipEntry& entry);

    // Get accessors
    wxDateTime   GetDateTime() const            { return m_DateTime; }
    wxFileOffset GetSize() const                { return m_Size; }
    wxFileOffset GetOffset() const              { return m_Offset; }
    wxString     GetInternalName() const        { return m_Name; }
    int          GetMethod() const              { return m_Method; }
    int          GetFlags() const               { return m_Flags; }
    wxUint32     GetCrc() const                 { return m_Crc; }
    wxFileOffset GetCompressedSize() const      { return m_CompressedSize; }
    int          GetSystemMadeBy() const        { return m_SystemMadeBy; }
    wxString     GetComment() const             { return m_Comment; }
    wxUint32     GetExternalAttributes() const  { return m_ExternalAttributes; }
    wxPathFormat GetInternalFormat() const      { return wxPATH_UNIX; }
    int          GetMode() const;
    const char  *GetLocalExtra() const;
    size_t       GetLocalExtraLen() const;
    const char  *GetExtra() const;
    size_t       GetExtraLen() const;
    wxString     GetName(wxPathFormat format = wxPATH_NATIVE) const;

    // is accessors
    inline bool IsDir() const;
    inline bool IsText() const;
    inline bool IsReadOnly() const;
    inline bool IsMadeByUnix() const;

    // set accessors
    void SetDateTime(const wxDateTime& dt)      { m_DateTime = dt; }
    void SetSize(wxFileOffset size)             { m_Size = size; }
    void SetMethod(int method)                  { m_Method = (wxUint16)method; }
    void SetComment(const wxString& comment)    { m_Comment = comment; }
    void SetExternalAttributes(wxUint32 attr )  { m_ExternalAttributes = attr; }
    void SetSystemMadeBy(int system);
    void SetMode(int mode);
    void SetExtra(const char *extra, size_t len);
    void SetLocalExtra(const char *extra, size_t len);

    inline void SetName(const wxString& name,
                        wxPathFormat format = wxPATH_NATIVE);

    static wxString GetInternalName(const wxString& name,
                                    wxPathFormat format = wxPATH_NATIVE,
                                    bool *pIsDir = NULL);

    // set is accessors
    void SetIsDir(bool isDir = true);
    inline void SetIsReadOnly(bool isReadOnly = true);
    inline void SetIsText(bool isText = true);

    C7ZipEntry *Clone() const                   { return ZipClone(); }

protected:
    // protected Get accessors
    int GetVersionNeeded() const                { return m_VersionNeeded; }
    wxFileOffset GetKey() const                 { return m_Key; }
    int GetVersionMadeBy() const                { return m_VersionMadeBy; }
    int GetDiskStart() const                    { return m_DiskStart; }
    int GetInternalAttributes() const           { return m_InternalAttributes; }

    void SetVersionNeeded(int version)          { m_VersionNeeded = (wxUint16)version; }
    void SetOffset(wxFileOffset offset)         { m_Offset = offset; }
    void SetFlags(int flags)                    { m_Flags = (wxUint16)flags; }
    void SetVersionMadeBy(int version)          { m_VersionMadeBy = (wxUint8)version; }
    void SetCrc(wxUint32 crc)                   { m_Crc = crc; }
    void SetCompressedSize(wxFileOffset size)   { m_CompressedSize = size; }
    void SetKey(wxFileOffset offset)            { m_Key = offset; }
    void SetDiskStart(int start)                { m_DiskStart = (wxUint16)start; }
    void SetInternalAttributes(int attr)        { m_InternalAttributes = (wxUint16)attr; }

    virtual C7ZipEntry *ZipClone() const        { return new C7ZipEntry(*this); }

    void Notify();

private:
    wxArchiveEntry* DoClone() const             { return ZipClone(); }

    size_t ReadLocal(wxInputStream& stream, wxMBConv& conv);
    size_t WriteLocal(wxOutputStream& stream, wxMBConv& conv) const;

    size_t ReadCentral(wxInputStream& stream, wxMBConv& conv);
    size_t WriteCentral(wxOutputStream& stream, wxMBConv& conv) const;

    size_t ReadDescriptor(wxInputStream& stream);
    size_t WriteDescriptor(wxOutputStream& stream, wxUint32 crc,
                           wxFileOffset compressedSize, wxFileOffset size);

    wxUint8      m_SystemMadeBy;       // one of enum C7ZipSystem
    wxUint8      m_VersionMadeBy;      // major * 10 + minor

    wxUint16     m_VersionNeeded;      // ver needed to extract (20 i.e. v2.0)
    wxUint16     m_Flags;
    wxUint16     m_Method;             // compression method (one of C7ZipMethod)
    wxDateTime   m_DateTime;
    wxUint32     m_Crc;
    wxFileOffset m_CompressedSize;
    wxFileOffset m_Size;
    wxString     m_Name;               // in internal format
    wxFileOffset m_Key;                // the original offset for copied entries
    wxFileOffset m_Offset;             // file offset of the entry

    wxString     m_Comment;
    wxUint16     m_DiskStart;          // for multidisk archives, not unsupported
    wxUint16     m_InternalAttributes; // bit 0 set for text files
    wxUint32     m_ExternalAttributes; // system specific depends on SystemMadeBy

	bool m_bIsDir;

    DECLARE_DYNAMIC_CLASS(C7ZipEntry)
};

WX_DECLARE_LIST_WITH_DECL(C7ZipEntry, C7ZipEntryList_, class WXDLLIMPEXP_BASE);

/////////////////////////////////////////////////////////////////////////////
// C7ZipEntry inlines

inline bool C7ZipEntry::IsText() const
{
    return false;
}

inline bool C7ZipEntry::IsDir() const
{
    return m_bIsDir;
}

inline bool C7ZipEntry::IsReadOnly() const
{
    return true;
}

inline bool C7ZipEntry::IsMadeByUnix() const
{
	return false;
}

inline void C7ZipEntry::SetIsText(bool isText)
{
}

inline void C7ZipEntry::SetIsReadOnly(bool isReadOnly)
{
    if (isReadOnly)
        SetMode(GetMode() & ~0222);
    else
        SetMode(GetMode() | 0200);
}

inline void C7ZipEntry::SetName(const wxString& name,
                                wxPathFormat format /*=wxPATH_NATIVE*/)
{
    bool isDir;
    m_Name = GetInternalName(name, format, &isDir);
    SetIsDir(isDir);
}

#endif