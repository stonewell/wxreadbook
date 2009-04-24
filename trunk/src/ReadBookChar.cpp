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
#include "ReadBookView.h"
#include "GBBig5Table.h"

CReadBookChar::CReadBookChar(CReadBookView * pView) :
m_pView(pView)
{
}

CReadBookChar::~CReadBookChar(void)
{
}

wxSize CReadBookChar::Paint(wxInt32 x, wxInt32 y, wxDC * pDC)
{
    wxSize size = pDC->GetTextExtent(m_nChar);

	if (m_pView->GetDisplayAs() != wxReadBook::DisplayAsOriginal)
	{
		pDC->DrawText(TranslateChar(m_nChar, m_pView->GetDisplayAs() == wxReadBook::DisplayAsSimplify), x, y);
	}
	else
	{
		pDC->DrawText(m_nChar, x, y);
	}

    return size;
}

bool CReadBookChar::IsAscii() const
{
    return m_nChar <= 0xFF && m_nChar >= 0;
}
