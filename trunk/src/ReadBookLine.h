#ifndef __READBOOK_LINE_H__
#define __READBOOK_LINE_H__

#include "ReadBookChar.h"

class CReadBookLine
{
public:
    CReadBookLine(void);
    virtual ~CReadBookLine(void);

private:

public:
    bool InsertChar(unsigned int nPos, const CReadBookChar * pChar);
    bool AppendChar(const CReadBookChar * pChar);
    bool RemoveChar(unsigned int nPos);
    CReadBookChar * GetChar(unsigned int nPos);

    int GetCharCount() const;
    wxFileOffset GetFileOffset() const;

    void Paint(int x, int y, wxDC * pDC, int count = -1);
};

#endif

