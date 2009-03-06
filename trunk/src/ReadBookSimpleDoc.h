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
	virtual bool ReadChar(wxChar & ch, 
        wxFileOffset & final_offset, 
        bool & end_of_line);
	virtual bool ReadCharAtOffset(wxFileOffset offset, 
        wxChar & ch, 
        wxFileOffset & final_offset, 
        bool & end_of_line);
	virtual wxFileOffset GetBufferSize(void) const;
    virtual wxFileOffset GetCurrentPosition(void) const;

	virtual void ShiftStream(wxInt32 delta);

protected:
	virtual bool LoadBuffer(const wxString & url, wxMBConv * conv, bool bGuess);

private:
	wxFileSystem m_FileSystem;
	wxFSFile * m_pFile;
	wxInputStream * m_pInput;
	wxMBConv * m_pConv;

	wxFileOffset m_nFileLength;
	
	char m_lastBytes[10];

	void CleanUp();
	wxChar NextChar();
	void UngetLast();
	bool EatEOL(const wxChar &c);
};