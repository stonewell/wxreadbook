#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/fontpicker.h"
#include "wx/clrpicker.h"
#include "wx/fontdlg.h"
#include "wx/colordlg.h"

#include "ReadBookApp.h"
#include "ReadBookPreferenceDlg.h"

BEGIN_EVENT_TABLE(CReadBookPreferenceDlg, wxDialog)
    EVT_BUTTON(wxID_ANY, CReadBookPreferenceDlg::OnButton)
    EVT_SPINCTRL(wxID_ANY, CReadBookPreferenceDlg::OnSpinChange)
END_EVENT_TABLE()

CReadBookPreferenceDlg::CReadBookPreferenceDlg(wxWindow * pParent) :
	wxDialog(pParent, wxID_ANY, wxT("wxReadBook Preference"), wxDefaultPosition),
	m_pFont(NULL)
{
	wxBoxSizer * pTopSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pNoteBook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_TOP);
	
	CreateAppearencePage();
	
	wxSizer * pSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);

	pTopSizer->Insert(0, m_pNoteBook, wxSizerFlags(5).Expand().Border());
	pTopSizer->Show(m_pNoteBook);

#ifndef _WIN32_WCE
	pTopSizer->Add(pSizer, 0, wxEXPAND | wxBOTTOM, 5);
#endif

	SetSizer(pTopSizer);

	pTopSizer->Layout();
    pTopSizer->SetSizeHints(this);
    pTopSizer->Fit(this);
}

CReadBookPreferenceDlg::~CReadBookPreferenceDlg()
{
	if (m_pFont != NULL) delete m_pFont;
}

void CReadBookPreferenceDlg::CreateAppearencePage()
{
    wxPanel *panel = new wxPanel(m_pNoteBook);

    //panel->SetBackgroundColour( wxColour( wxT("MAROON") ) );
    
    m_pStaticText = new wxStaticText(panel, wxID_ANY, 
    		wxT("Sample"), 
    		wxDefaultPosition, 
    		wxDefaultSize, 
    		wxALIGN_CENTRE | wxBORDER_SIMPLE | wxST_NO_AUTORESIZE);
    
    SetFont(wxGetApp().GetPreference()->GetFont());
    m_cBkColor = wxGetApp().GetPreference()->GetBackgroundColor();
    m_cTxtColor = wxGetApp().GetPreference()->GetTextColor();
    
    m_pStaticText->SetFont(*m_pFont);
    m_pStaticText->SetBackgroundColour(m_cBkColor);
    m_pStaticText->SetForegroundColour(m_cTxtColor);
    
    wxBoxSizer *pSizer = new wxBoxSizer( wxVERTICAL );

    // create text ctrl with minimal size 100x60
    pSizer->Add(
      m_pStaticText,
      1,          // make vertically stretchable
      wxEXPAND | // make horizontally stretchable 
      wxALL,   //   and make border all around
      10 );        // set border width to 10

    wxBoxSizer *pBtnSizer = new wxBoxSizer( wxHORIZONTAL );
    
    m_pBtnFont = new wxButton( panel, wxID_ANY, wxT("Font..."));
    m_pBtnTxtColor = new wxButton( panel, wxID_ANY, wxT("Text Color...") );
    m_pBtnBkColor = new wxButton( panel, wxID_ANY, wxT("Background Color...") );
    
    pBtnSizer->Add(
       m_pBtnFont,
       0,         // make horizontally unstretchable
       wxALL,      // make border all around (implicit top alignment)
       10 );       // set border width to 10
    pBtnSizer->Add(
       m_pBtnTxtColor,
       0,         // make horizontally unstretchable
       wxALL,      // make border all around (implicit top alignment)
       10 );       // set border width to 10
    pBtnSizer->Add(
       m_pBtnBkColor,
       0,         // make horizontally unstretchable
       wxALL,      // make border all around (implicit top alignment)
       10 );       // set border width to 10

    pSizer->Add(
       pBtnSizer,
       0,             // make vertically unstretchable
       wxALIGN_CENTER ); // no border and centre horizontally

    wxRect rect = GetParent()->GetClientRect();
    
    m_nLineMargin = wxGetApp().GetPreference()->GetLineMargin();
    m_nColumnMargin = wxGetApp().GetPreference()->GetColumnMargin();

    m_pLineMargin = new wxSpinCtrl(panel,
    		wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
    		wxSP_ARROW_KEYS | wxSP_WRAP, 0, rect.GetHeight() / 2,
    		m_nLineMargin);
    
    m_pColumnMargin = new wxSpinCtrl(panel,
    		wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
    		wxSP_ARROW_KEYS | wxSP_WRAP, 0, rect.GetWidth() / 2,
    		m_nColumnMargin);

    wxBoxSizer *pMarginSizer = new wxBoxSizer( wxHORIZONTAL );
   
    pMarginSizer->Add(
       new wxStaticText(panel, wxID_ANY,wxT("Line Margin:"),wxDefaultPosition, 
       		wxDefaultSize, 
       		wxALIGN_RIGHT),
       0,         // make horizontally unstretchable
       wxLeft | wxALIGN_RIGHT | wxALIGN_CENTER,      // make border all around (implicit top alignment)
       10 );       // set border width to 10

    pMarginSizer->Add(
       m_pLineMargin,
       0,         // make horizontally unstretchable
       wxALL,      // make border all around (implicit top alignment)
       10 );       // set border width to 10

    pMarginSizer->Add(
       new wxStaticText(panel, wxID_ANY,wxT("Page Margin:"),wxDefaultPosition, 
       		wxDefaultSize, 
       		wxALIGN_RIGHT),
       0,         // make horizontally unstretchable
       wxALL | wxALIGN_RIGHT | wxALIGN_CENTER,      // make border all around (implicit top alignment)
       0 );       // set border width to 10

    pMarginSizer->Add(
       m_pColumnMargin,
       0,         // make horizontally unstretchable
       wxALL,      // make border all around (implicit top alignment)
       10 );       // set border width to 10

    pSizer->Add(
       pMarginSizer,
       0,             // make vertically unstretchable
       wxALIGN_CENTER ); // no border and centre horizontally

    panel->SetSizer( pSizer );     // use the sizer for layout

    pSizer->SetSizeHints( panel );   // set size hints to honour minimum size    
    
    m_pNoteBook->InsertPage( 0, panel, wxT("Appearence"), false, -1 );
}

void CReadBookPreferenceDlg::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == m_pBtnFont )
    {
    	wxFont * pOldFont = 
    		wxGetApp().GetPreference()->GetFont();
    	
    	wxFont newFont =
    		wxGetFontFromUser(this,*pOldFont);
    	
    	if (newFont.IsOk())
    	{
    		m_pStaticText->SetFont(newFont);
    		m_pStaticText->Refresh();
    		this->Fit();
    		
    		SetFont(&newFont);
    	}
    }
    else if ( event.GetEventObject() == m_pBtnTxtColor )
    {
        wxColour m_cTxtColor = wxGetApp().GetPreference()->GetTextColor();
        
        wxColour newColor = wxGetColourFromUser(this, m_cTxtColor);
        
        if (newColor.IsOk()) 
        {
        	m_pStaticText->SetForegroundColour(newColor);
    		m_pStaticText->Refresh();
    		
    		SetTextColor(newColor);
        }
    }
    else if ( event.GetEventObject() == m_pBtnBkColor )
    {
        wxColour m_cBkColor = wxGetApp().GetPreference()->GetBackgroundColor();
        
        wxColour newColor = wxGetColourFromUser(this, m_cBkColor);
        
        if (newColor.IsOk()) 
        {
        	m_pStaticText->SetBackgroundColour(newColor);
    		m_pStaticText->Refresh();
    		
    		SetBackgroundColor(newColor);
        }
    }
    else
    {
        event.Skip();
    }
}

void CReadBookPreferenceDlg::SetFont(wxFont * pFont)
{
	if (m_pFont != NULL) delete m_pFont;
	
	m_pFont = new wxFont(*pFont);
}

void CReadBookPreferenceDlg::OnSpinChange(wxSpinEvent& event)
{
	if ( event.GetEventObject() == m_pLineMargin )
	{
		m_nLineMargin = m_pLineMargin->GetValue();
	}
	else if ( event.GetEventObject() == m_pColumnMargin )
	{
		m_nColumnMargin = m_pColumnMargin->GetValue();
	}
}
