#ifndef __7ZIP_INPUT_STREAM_H__
#define __7ZIP_INPUT_STREAM_H__

/////////////////////////////////////////////////////////////////////////////
// C7ZipInputStream
#include "lib7zip.h"

class WXDLLIMPEXP_BASE C7ZipInputStream : public wxArchiveInputStream
{
public:
    typedef C7ZipEntry entry_type;

    C7ZipInputStream(wxInputStream& stream, wxMBConv& conv = wxConvLocal);
    C7ZipInputStream(wxInputStream *stream, wxMBConv& conv = wxConvLocal);

    virtual ~C7ZipInputStream();

protected:
    virtual size_t OnSysRead(void *buffer, size_t size);

    // this protected interface isn't yet finalised
    virtual wxInputStream* OpenDecompressor(wxInputStream& stream);
    virtual bool CloseDecompressor(wxInputStream *decomp);
    virtual wxArchiveEntry *DoGetNextEntry()    { return GetNextEntry(); }
	virtual bool OpenEntry(wxArchiveEntry& entry);
    virtual bool CloseEntry();
    virtual wxFileOffset GetLength() const;

private:
    void Init();
    C7ZipEntry *GetNextEntry();

	size_t m_nCurrentEntryIndex;
	C7ZipLibrary m_7ZipLibrary;
	C7ZipArchive * m_p7ZipArchive;

    DECLARE_NO_COPY_CLASS(C7ZipInputStream)
};


#endif