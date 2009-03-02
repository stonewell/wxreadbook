#ifndef __READBOOK_PAGE_H__
#define __READBOOK_PAGE_H__

#include "ReadBookLine.h"

class CReadBookPage
{
public:
    CReadBookPage(void);
    virtual ~CReadBookPage(void);

private:

public:
    bool InsertChar(unsigned int nRow, unsigned int nCol, const CReadBookChar * pChar);
    bool AppendChar(const CReadBookChar * pChar);
    bool RemoveChar(unsigned int nRow, unsigned int nCol);

    int GetLineCount() const;

    CReadBookLine * GetLine(unsigned int nRow);
    CReadBookChar * GetChar(unsigned int nRow, unsigned int nCol);

    wxFileOffset GetFileOffset() const;

    void Paint(int x, int y, wxDC * pDC);
};

#endif
