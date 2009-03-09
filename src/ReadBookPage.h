#ifndef __READBOOK_PAGE_H__
#define __READBOOK_PAGE_H__

#include "ReadBookLine.h"

class CReadBookPage
{
public:
    CReadBookPage(wxUint32 nMaxLineCount,
        wxUint32 nLineHeight,
        wxUint32 nLineMargin,
        wxUint32 nMaxAsciiCharCount,
        wxUint32 nAvgAsciiCharWidth,
        wxUint32 nColMargin);
    virtual ~CReadBookPage(void);

private:
    wxUint32 m_nMaxLineCount;
    wxUint32 m_nLineHeight;
    wxUint32 m_nLineMargin;

    wxUint32 m_nMaxAsciiCharCount;
    wxUint32 m_nAvgAsciiCharWidth;
    wxUint32 m_nColMargin;

    CReadBookLine ** m_ReadBookLines;

    CReadBookLine * CreateLine(wxUint32 nRow);

public:
    bool SetChar(wxUint32 nRow, wxUint32 nCol, const CReadBookChar * pChar);
    bool AppendChar(const CReadBookChar * pChar);
    bool RemoveChar(wxUint32 nRow, wxUint32 nCol, bool destroy = true);

    bool SetLine(wxUint32 nRow, const CReadBookLine * pLine);
    bool RemoveLine(wxUint32 nRow, bool destroy = true);

    wxInt32 GetLineCount() const;

    CReadBookLine * GetLine(wxUint32 nRow) const;
    CReadBookChar * GetChar(wxUint32 nRow, wxUint32 nCol) const;

    wxFileOffset GetBeginFileOffset() const;
    wxFileOffset GetEndFileOffset() const;

    void Paint(wxInt32 x, wxInt32 y, wxDC * pDC);

    wxUint32 GetLineHeight() const { return m_nLineHeight; }

    wxUint32 GetLineMargin() const { return m_nLineMargin; }

    void TrimEmptyLines();
};

#endif
