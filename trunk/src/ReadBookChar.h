#ifndef __READBOOK_CHAR_H__
#define __READBOOK_CHAR_H__

class CReadBookView;

class CReadBookChar
{
private:
	friend class CReadBookCharCache;
    CReadBookChar(CReadBookView * pView);
    virtual ~CReadBookChar(void);

private:
	CReadBookView * m_pView;
    wxFileOffset m_nBeginFileOffset;
    wxFileOffset m_nEndFileOffset;
    wxChar m_nChar;

public:
    wxFileOffset GetBeginFileOffset() const { return m_nBeginFileOffset; }
    wxFileOffset GetEndFileOffset() const { return m_nEndFileOffset; }
    wxChar GetChar() const { return m_nChar; }

    void SetData(wxFileOffset begin_offset, wxFileOffset end_offset, wxChar ch) 
    { 
        m_nBeginFileOffset = begin_offset; 
        m_nEndFileOffset = end_offset;
        m_nChar = ch; 
    }

    wxSize Paint(wxInt32 x, wxInt32 y, wxDC * pDC);

    bool IsAscii() const;

	void Init(CReadBookView * pView);
	void Release();
};

#endif
