#ifndef __7ZIP_INPUT_STREAM_H__
#define __7ZIP_INPUT_STREAM_H__

/////////////////////////////////////////////////////////////////////////////
// C7ZipInputStream
#include "lib7zip.h"

class C7ZipInStreamImpl : public virtual C7ZipInStream
{
public:
	C7ZipInStreamImpl(wxString strExt, wxInputStream * stream) : 
	  m_pInStream(stream),
	  m_strExt(strExt)
	{
	}

	virtual ~C7ZipInStreamImpl() {}

	virtual wstring GetExt() const;
	virtual int Read(void *data, unsigned int size, unsigned int *processedSize);
	virtual int Seek(__int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition);
	virtual int GetSize(unsigned __int64 * size);

private:
	wxInputStream * m_pInStream;
	wxString m_strExt;
};

class WXDLLIMPEXP_BASE C7ZipInputStream : public wxArchiveInputStream
{
public:
    typedef C7ZipEntry entry_type;

    C7ZipInputStream(wxString strExt, wxInputStream& stream, wxMBConv& conv = wxConvLocal);
    C7ZipInputStream(wxString strExt, wxInputStream *stream, wxMBConv& conv = wxConvLocal);

    virtual ~C7ZipInputStream();

protected:
    virtual size_t OnSysRead(void *buffer, size_t size);
    virtual wxFileOffset OnSysSeek(wxFileOffset seek, wxSeekMode mode);
	virtual wxFileOffset OnSysTell() const;
	virtual bool IsSeekable() const { return true; }

    // this protected interface isn't yet finalised
    virtual wxInputStream* OpenDecompressor(wxInputStream& stream);
    virtual bool CloseDecompressor(wxInputStream *decomp);
    virtual wxArchiveEntry *DoGetNextEntry()    { return GetNextEntry(); }
	virtual bool OpenEntry(wxArchiveEntry& entry);
    virtual bool CloseEntry();
    //virtual wxFileOffset GetLength() const;

    virtual bool Eof() const;
private:
    void Init();
    C7ZipEntry *GetNextEntry();
	bool OpenEntry(C7ZipEntry & entry);

	size_t m_nCurrentEntryIndex;
	C7ZipLibrary m_7ZipLibrary;
	C7ZipArchive * m_p7ZipArchive;
	C7ZipEntryList_ m_Entries;
	C7ZipInStreamImpl m_InStreamImpl;

	wxInputStream * m_pEntryStream;

	wxString m_strExt;

    DECLARE_NO_COPY_CLASS(C7ZipInputStream)
};


#endif