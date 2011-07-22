#include "wx/fontdlg.h" 


// 
//----------------------------------------------------------------------------- 
// wxRichTextFontDialog: a substitute for wxFontDialog 
// 
//----------------------------------------------------------------------------- 


class wxRichTextFontDialog: public wxFontDialogBase 
{ 
public: 
    wxRichTextFontDialog() : wxFontDialogBase() { 
		Init(); /* must be Create() later */
	} 
    wxRichTextFontDialog(wxWindow *parent) 
        : wxFontDialogBase(parent) { Init(); Create(parent); } 
    wxRichTextFontDialog(wxWindow *parent, const wxFontData& data) 
        : wxFontDialogBase(parent, data) { Init(); Create(parent, data); } 


    void Init() { m_title = _("Font"); } 


    virtual int ShowModal(); 


    virtual void SetTitle( const wxString& title) { m_title = title; } 
    virtual wxString GetTitle() const { return m_title; } 


protected: 
    wxString    m_title; 


    DECLARE_DYNAMIC_CLASS_NO_COPY(wxRichTextFontDialog) 
};
