#ifndef __READBOOK_CHAR_H__
#define __READBOOK_CHAR_H__

class CReadBookChar
{
public:
    CReadBookChar(void);
    virtual ~CReadBookChar(void);

private:
    wxFileOffset m_nFileOffset;
    wxChar m_nChar;

public:
    wxFileOffset GetFileOffset() const { return m_nFileOffset; }
    wxChar GetChar() const { return m_nChar; }

    void SetData(wxFileOffset offset, wxChar ch) { m_nFileOffset = offset; m_nChar = ch; }

    void Paint(wxInt32 x, wxInt32 y, wxDC * pDC);

    bool IsAscii() const;
};

#endif
