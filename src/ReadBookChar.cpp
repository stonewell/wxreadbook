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
#include "ReadBookChar.h"

CReadBookChar::CReadBookChar(void)
{
}

CReadBookChar::~CReadBookChar(void)
{
}

wxSize CReadBookChar::Paint(wxInt32 x, wxInt32 y, wxDC * pDC)
{
    wxSize size = pDC->GetTextExtent(m_nChar);
    pDC->DrawText(m_nChar, x, y);

    return size;
}

bool CReadBookChar::IsAscii() const
{
    return m_nChar <= 0xFF && m_nChar >= 0;
}
