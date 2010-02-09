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

#include <wx/dcclient.h>

#include "../ReadBookApp.h"
#include "ReadBookDC.h"

CReadBookDC::CReadBookDC(wxWindow * pWindow) : wxClientDC(pWindow)
{
	m_OldFont = GetFont();

	wxFont * pNewFont = wxGetApp().GetPreference()->GetFont();
	SetFont(*pNewFont);

	m_OldTextColor = GetTextForeground();

	SetTextForeground(wxGetApp().GetPreference()->GetTextColor());

	m_OldBkColor = GetBackground();

	SetBackground(wxBrush(wxGetApp().GetPreference()->GetBackgroundColor()));
}

CReadBookDC::~CReadBookDC(void)
{
	SetFont(m_OldFont);
	SetTextForeground(m_OldTextColor);
	SetBackground(m_OldBkColor);
}
