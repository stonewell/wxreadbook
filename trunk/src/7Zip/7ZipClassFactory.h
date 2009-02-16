#ifndef __7ZIP_CLASS_FACTORY_H__
#define __7ZIP_CLASS_FACTORY_H__

/////////////////////////////////////////////////////////////////////////////
// 7ZIPClassFactory

class WXDLLIMPEXP_BASE C7ZipClassFactory : public wxArchiveClassFactory
{
public:
    C7ZipClassFactory();

    C7ZipEntry *NewEntry() const
        { return new C7ZipEntry; }
    C7ZipInputStream *NewStream(wxInputStream& stream) const
        { return new C7ZipInputStream(stream, GetConv()); }
    C7ZipOutputStream *NewStream(wxOutputStream& stream) const
        { return new C7ZipOutputStream(stream, -1, GetConv()); }
    C7ZipInputStream *NewStream(wxInputStream *stream) const
        { return new C7ZipInputStream(stream, GetConv()); }
    C7ZipOutputStream *NewStream(wxOutputStream *stream) const
        { return new C7ZipOutputStream(stream, -1, GetConv()); }

    wxString GetInternalName(const wxString& name,
                             wxPathFormat format = wxPATH_NATIVE) const
        { return C7ZipEntry::GetInternalName(name, format); }

    const wxChar * const *GetProtocols(wxStreamProtocolType type
                                       = wxSTREAM_PROTOCOL) const;

protected:
    wxArchiveEntry *DoNewEntry() const
        { return NewEntry(); }
    wxArchiveInputStream *DoNewStream(wxInputStream& stream) const
        { return NewStream(stream); }
    wxArchiveOutputStream *DoNewStream(wxOutputStream& stream) const
        { return NewStream(stream); }
    wxArchiveInputStream *DoNewStream(wxInputStream *stream) const
        { return NewStream(stream); }
    wxArchiveOutputStream *DoNewStream(wxOutputStream *stream) const
        { return NewStream(stream); }

private:
    DECLARE_DYNAMIC_CLASS(C7ZipClassFactory)
};

#endif
