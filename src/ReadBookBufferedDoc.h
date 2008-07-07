#pragma once

#include "ReadBookDoc.h"
#include <vector>
typedef std::vector<wxFileOffset> wxArrayFileOffset;

#include <map>
typedef std::map<wxInt32, wxFileOffset> wxMapFileOffset;

class CReadBookBufferedDoc :
	public CReadBookDoc
{
    DECLARE_DYNAMIC_CLASS(CReadBookBufferedDoc)

public:
	CReadBookBufferedDoc(void);
	virtual ~CReadBookBufferedDoc(void);

public:
	virtual const wxString & GetLine(wxInt32 nRow);
	virtual wxInt32 GetBufferSize(void) const;

	virtual wxInt32 GetBufferCurPos(void);
	
	virtual const wxInt32 GetCharsPerLine() const { return m_nCharsPerLine; };
	virtual void SetCharsPerLine(wxInt32 charsPerLine);

	virtual void ShiftStream(wxInt32 delta);

	virtual wxInt32 OffsetToRow(wxInt32 nOffset);
	wxFileOffset RowToOffset(wxInt32 nRow);

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

	wxInt32 m_nCharsPerLine;
	wxFileOffset m_nFileLength;
	
	wxInt32 m_nLastReadRow;

	char m_lastBytes[10];

	CIntStringMap m_LinesMapping;

	wxMapFileOffset m_RowOffsetMap;

	void CleanUp();
	wxChar NextChar();
	void UngetLast();
	bool EatEOL(const wxChar &c);
	void SaveLine(wxInt32 nRow, const wxString & strLine);
	wxInt32 FindNearRow(wxInt32 nRow);
};
