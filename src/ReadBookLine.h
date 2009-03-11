#ifndef __READBOOK_LINE_H__
#define __READBOOK_LINE_H__

#include "ReadBookChar.h"

class CReadBookLine
{
public:
    CReadBookLine(wxInt32 nLineWidth,
        wxInt32 nMaxAsciiCharCount,
        wxInt32 nAvgAsciiCharWidth,
        wxInt32 nColMargin);
    virtual ~CReadBookLine(void);

private:
    wxInt32 m_nLineWidth;
    wxInt32 m_nMaxAsciiCharCount;
    wxInt32 m_nAvgAsciiCharWidth;
    wxInt32 m_nColMargin;

    CReadBookChar ** m_ReadBookChars;

public:
    bool SetChar(wxInt32 nPos, const CReadBookChar * pChar, wxDC * pDC);
    bool AppendChar(const CReadBookChar * pChar, wxDC * pDC);
    bool RemoveChar(wxInt32 nPos,bool destroy = true);
    CReadBookChar * GetChar(wxInt32 nPos);

    wxInt32 GetAsciiCharCount() const;

    wxFileOffset GetBeginFileOffset() const;
    wxFileOffset GetEndFileOffset() const;

    void Paint(wxInt32 x, wxInt32 y, wxDC * pDC, wxInt32 count = -1);
    void TrimEmptyChars();
};

#endif

