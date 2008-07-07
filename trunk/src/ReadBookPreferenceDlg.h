#ifndef READBOOKPREFERENCEDLG_H_
#define READBOOKPREFERENCEDLG_H_

#include "wx/dialog.h"
#include "wx/notebook.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/spinctrl.h"

class CReadBookPreferenceDlg : public wxDialog
{
public:
	CReadBookPreferenceDlg(wxWindow * pParent);
	virtual ~CReadBookPreferenceDlg();
	
    void OnButton(wxCommandEvent& event);
    void OnSpinChange(wxSpinEvent& event);
    
private:
	wxNotebook * m_pNoteBook;
	wxStaticText * m_pStaticText;
	wxButton * m_pBtnFont;
	wxButton * m_pBtnTxtColor;
	wxButton * m_pBtnBkColor;
	
	wxFont * m_pFont;
	wxColour m_cTxtColor;
	wxColour m_cBkColor;
	
	wxSpinCtrl * m_pLineMargin;
	wxSpinCtrl * m_pColumnMargin;
	
	wxInt16 m_nLineMargin;
	wxInt16 m_nColumnMargin;
	
	void CreateAppearencePage();

    DECLARE_EVENT_TABLE()

public:
	wxColour GetTextColor() const { return m_cTxtColor; }
	void SetTextColor(wxColour color) { m_cTxtColor = color; }
	
	wxColour GetBackgroundColor() const { return m_cBkColor; }
	void SetBackgroundColor(wxColour color) { m_cBkColor = color; }

	void SetFont(wxFont * pFont);
	wxFont * GetFont() const { return m_pFont; }
	
	wxInt16 GetLineMargin() const { return m_nLineMargin; }
	void SetLineMargin(wxInt16 nLineMargin) { m_nLineMargin = nLineMargin; }

	wxInt16 GetColumnMargin() const { return m_nColumnMargin; }
	void SetColumnMargin(wxInt16 nColumnMargin) { m_nColumnMargin = nColumnMargin; }
};

#endif /*READBOOKPREFERENCEDLG_H_*/
