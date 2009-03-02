#pragma once

#include "ReadBookDoc.h"

class CReadBookSimpleDoc :
	public CReadBookDoc
{
    DECLARE_DYNAMIC_CLASS(CReadBookSimpleDoc)

public:
	CReadBookSimpleDoc(void);
	virtual ~CReadBookSimpleDoc(void);

public:
	virtual const wxString & GetLine(wxInt32 nRow);
	virtual wxInt32 GetBufferSize(void) const;

	virtual void ShiftStream(wxInt32 delta);

	virtual wxInt32 OffsetToRow(wxInt32 nOffset);
	virtual wxFileOffset RowToOffset(wxInt32 nRow);

protected:
	virtual wxInt32 GetOffsetRow(wxFileOffset nOffset);
	virtual bool LoadBuffer(const wxString & url, wxMBConv * conv, bool bGuess);
	virtual wxFileOffset GetRowOffset(wxInt32 nRow);

private:
	WX_DECLARE_HASH_MAP( wxInt32, wxString, wxIntegerHash, wxIntegerEqual, CIntStringMap);

	wxFileSystem m_FileSystem;
	wxFSFile * m_pFile;
	wxInputStream * m_pInput;
	wxMBConv * m_pConv;

	wxFileOffset m_nFileLength;
	
	wxInt32 m_nLastReadRow;

	char m_lastBytes[10];

	CIntStringMap m_LinesMapping;

	void CleanUp();
	wxChar NextChar();
	void UngetLast();
	bool EatEOL(const wxChar &c);
	void SaveLine(wxInt32 nRow, const wxString & strLine);
	wxInt32 FindNearRow(wxInt32 nRow);
};
