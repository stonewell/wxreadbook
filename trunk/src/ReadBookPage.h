#ifndef __READBOOK_PAGE_H__
#define __READBOOK_PAGE_H__

#include "ReadBookLine.h"

class CReadBookPage
{
public:
    CReadBookPage(wxInt32 nMaxLineCount,
        wxInt32 nLineWidth,
        wxInt32 nLineHeight,
        wxInt32 nLineMargin,
        wxInt32 nMaxAsciiCharCount,
        wxInt32 nAvgAsciiCharWidth,
        wxInt32 nColMargin);
    virtual ~CReadBookPage(void);

private:
    wxInt32 m_nMaxLineCount;
    wxInt32 m_nLineWidth;
    wxInt32 m_nLineHeight;
    wxInt32 m_nLineMargin;

    wxInt32 m_nMaxAsciiCharCount;
    wxInt32 m_nAvgAsciiCharWidth;
    wxInt32 m_nColMargin;

    CReadBookLine ** m_ReadBookLines;

    CReadBookLine * CreateLine(wxInt32 nRow);

public:
    bool SetChar(wxInt32 nRow, wxInt32 nCol, const CReadBookChar * pChar, wxDC * pDC);
    bool AppendChar(const CReadBookChar * pChar, wxDC * pDC);
    bool RemoveChar(wxInt32 nRow, wxInt32 nCol, bool destroy = true);

    bool SetLine(wxInt32 nRow, const CReadBookLine * pLine);
    bool RemoveLine(wxInt32 nRow, bool destroy = true);

    wxInt32 GetLineCount() const;

    CReadBookLine * GetLine(wxInt32 nRow) const;
    CReadBookChar * GetChar(wxInt32 nRow, wxInt32 nCol) const;

    wxFileOffset GetBeginFileOffset() const;
    wxFileOffset GetEndFileOffset() const;

    void Paint(wxInt32 x, wxInt32 y, wxDC * pDC);

    wxInt32 GetLineHeight() const { return m_nLineHeight; }

    wxInt32 GetLineMargin() const { return m_nLineMargin; }

    void TrimEmptyLines();
    void NormalizeLines();
};

#endif
