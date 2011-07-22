#include "wx/wx.h"
#include "wx/richtext/richtextformatdlg.h" 
#include "richtextfontdlg.h"


IMPLEMENT_DYNAMIC_CLASS(wxRichTextFontDialog, wxDialog) 


int wxRichTextFontDialog::ShowModal() 
{ 
    wxTextAttrEx attr; 
    if (m_fontData.GetInitialFont().Ok()) 
        attr.SetFont(m_fontData.GetInitialFont()); 


    if (m_fontData.GetColour().Ok()) 
        attr.SetTextColour(m_fontData.GetColour()); 


    wxRichTextFormattingDialog formatDlg(wxRICHTEXT_FORMAT_FONT, 
GetParent(), GetTitle()); 
    formatDlg.SetAttributes(attr); 


    if (formatDlg.ShowModal() == wxID_OK) 
    { 
        wxTextAttrEx attr(formatDlg.GetAttributes()); 


        m_fontData.SetChosenFont(attr.GetFont()); 
        m_fontData.SetColour(attr.GetTextColour()); 


        return wxID_OK; 
    } 
    else 
        return wxID_CANCEL; 



} 
