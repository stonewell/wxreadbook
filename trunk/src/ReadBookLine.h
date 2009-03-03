#ifndef __READBOOK_LINE_H__
#define __READBOOK_LINE_H__

#include "ReadBookChar.h"

class CReadBookLine
{
public:
    CReadBookLine(wxUint32 nMaxAsciiCharCount,
        wxUint32 nAvgAsciiCharWidth,
        wxUint32 nColMargin);
    virtual ~CReadBookLine(void);

private:
    wxUint32 m_nMaxAsciiCharCount;
    wxUint32 m_nAvgAsciiCharWidth;
    wxUint32 m_nColMargin;

    CReadBookChar ** m_ReadBookChars;

public:
    bool SetChar(wxUint32 nPos, const CReadBookChar * pChar);
    bool AppendChar(const CReadBookChar * pChar);
    bool RemoveChar(wxUint32 nPos,bool destroy = true);
    CReadBookChar * GetChar(wxUint32 nPos);

    wxUint32 GetAsciiCharCount() const;

    wxFileOffset GetFileOffset() const;

    void Paint(wxInt32 x, wxInt32 y, wxDC * pDC, wxInt32 count = -1);
};

#endif

