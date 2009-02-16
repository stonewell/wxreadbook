#ifndef __7ZIP_OUTPUT_STREAM_H__
#define __7ZIP_OUTPUT_STREAM_H__

class C7ZipInputStream;

class WXDLLIMPEXP_BASE C7ZipOutputStream : public wxArchiveOutputStream
{
public:
	C7ZipOutputStream(wxOutputStream& stream,
		int level = -1,
		wxMBConv& conv = wxConvLocal);
	C7ZipOutputStream(wxOutputStream *stream,
		int level = -1,
		wxMBConv& conv = wxConvLocal);
	virtual ~C7ZipOutputStream();

	bool PutNextEntry(C7ZipEntry *entry)        { return DoCreate(entry); }

	bool PutNextEntry(const wxString& name,
		const wxDateTime& dt = wxDateTime::Now(),
		wxFileOffset size = wxInvalidOffset);

	bool PutNextDirEntry(const wxString& name,
		const wxDateTime& dt = wxDateTime::Now());

	bool CopyEntry(C7ZipEntry *entry, C7ZipInputStream& inputStream);
	bool CopyArchiveMetaData(C7ZipInputStream& inputStream);

	void Sync();
	bool CloseEntry();
	bool Close();

	void SetComment(const wxString& comment)    { m_Comment = comment; }

	int  GetLevel() const                       { return m_level; }
	void SetLevel(int level);

protected:
	virtual size_t OnSysWrite(const void *buffer, size_t size);
	virtual wxFileOffset OnSysTell() const      { return m_entrySize; }

	// this protected interface isn't yet finalised
	struct Buffer { const char *m_data; size_t m_size; };
	virtual wxOutputStream* OpenCompressor(wxOutputStream& stream,
		C7ZipEntry& entry,
		const Buffer bufs[]);
	virtual bool CloseCompressor(wxOutputStream *comp);

	bool IsParentSeekable() const
	{ return m_offsetAdjustment != wxInvalidOffset; }

private:
	void Init(int level);

	bool PutNextEntry(wxArchiveEntry *entry);
	bool CopyEntry(wxArchiveEntry *entry, wxArchiveInputStream& stream);
	bool CopyArchiveMetaData(wxArchiveInputStream& stream);

	bool IsOpened() const { return m_comp || m_pending; }

	bool DoCreate(C7ZipEntry *entry, bool raw = false);
	void CreatePendingEntry(const void *buffer, size_t size);
	void CreatePendingEntry();

	C7ZipEntryList_ m_entries;
	char *m_initialData;
	size_t m_initialSize;
	C7ZipEntry *m_pending;
	bool m_raw;
	wxFileOffset m_headerOffset;
	size_t m_headerSize;
	wxFileOffset m_entrySize;
	wxUint32 m_crcAccumulator;
	wxOutputStream *m_comp;
	int m_level;
	wxFileOffset m_offsetAdjustment;
	wxString m_Comment;

	DECLARE_NO_COPY_CLASS(C7ZipOutputStream)
};

#endif