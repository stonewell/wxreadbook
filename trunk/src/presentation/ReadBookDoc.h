#ifndef READBOOKDOC_H_
#define READBOOKDOC_H_

#include "wx/docview.h"

class CReadBookDoc : public wxDocument
{
	DECLARE_DYNAMIC_CLASS(CReadBookDoc)

private:
	wxString m_strFileName;
	wxInt32 m_nCurrentLine;

public:
	CReadBookDoc(void);
	virtual ~CReadBookDoc(void);

public:
	virtual bool OnCreate(const wxString& path, long flags);
	virtual bool OnNewDocument();
	virtual bool OnSaveDocument(const wxString& filename);
	virtual bool OnOpenDocument(const wxString& filename);
	virtual bool IsModified(void) const;
	virtual bool OnCloseDocument();
	virtual void Modify(bool mod);

public:
	const wxString & GetFileName() const { return m_strFileName; }

	wxInt32 GetCurrentLine() const { return m_nCurrentLine; }
	void SetCurrentLine(wxInt32 nCurrentLine);

	bool IsEmptyLine(const wxString & line) const;

	virtual const wxString & GetLine(wxInt32 nRow);

	virtual wxFileOffset GetBufferSize(void) const;

	void ReloadByEncoding(wxMBConv * conv);
	void ReloadCurrentDocument();

	void UpdateDisplay(wxReadBook::DisplayAsEnum displayAs,
		wxReadBook::ViewModeEnum viewMode);

protected:
	virtual bool OpenDocument(const wxString & filename, wxMBConv * conv, bool bGuess);
	virtual bool LoadBuffer(const wxString & url, wxMBConv * conv, bool bGuess);

	virtual wxMBConv * GetSuitableMBConv(wxInputStream * pInput, wxMBConv * pDefaultConv, bool bGuess);
	virtual wxFileOffset GetRowOffset(wxInt32 nRow);

	wxArrayString m_Buffer;
};

#endif /*READBOOKDOC_H_*/
