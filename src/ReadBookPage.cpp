#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "ReadBookApp.h"
#include "ReadBookPage.h"

CReadBookPage::CReadBookPage(void)
{
}

CReadBookPage::~CReadBookPage(void)
{
}

bool CReadBookPage::InsertChar(unsigned int nRow, unsigned int nCol, const CReadBookChar * pChar)
{
}

bool CReadBookPage::AppendChar(const CReadBookChar * pChar)
{
}

bool CReadBookPage::RemoveChar(unsigned int nRow, unsigned int nCol)
{
}

int CReadBookPage::GetLineCount() const
{
}

CReadBookLine * CReadBookPage::GetLine(unsigned int nRow)
{
}

CReadBookChar * CReadBookPage::GetChar(unsigned int nRow, unsigned int nCol)
{
}

wxFileOffset CReadBookPage::GetFileOffset() const
{
}

void CReadBookPage::Paint(int x, int y, wxDC * pDC)
{
}
