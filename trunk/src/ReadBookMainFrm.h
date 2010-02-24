#ifndef READBOOKMAINFRM_H_
#define READBOOKMAINFRM_H_

class CReadBookCanvas;

class CReadBookMainFrm: public wxDocParentFrame
{
	DECLARE_CLASS(CReadBookMainFrm)

public:
	CReadBookMainFrm(wxDocManager *manager,
		wxFrame *frame, wxWindowID id,
		const wxString& title,
		const wxPoint& pos,
		const wxSize& size,
		const long type);

	virtual ~CReadBookMainFrm();

	void OnAbout(wxCommandEvent& event);
	void OnPreference(wxCommandEvent& event);
	void OnGoto(wxCommandEvent& event);
	void OnViewAsHtml(wxCommandEvent& event);
	void OnViewAsText(wxCommandEvent& event);
	void OnViewDisplayOriginal(wxCommandEvent& event);
	void OnViewDisplaySimplify(wxCommandEvent& event);
	void OnViewDisplayTraditional(wxCommandEvent& event);
	void OnViewAsHtmlUpdateUI(wxUpdateUIEvent& event);
	void OnViewAsTextUpdateUI(wxUpdateUIEvent& event);
	void OnViewDisplayOriginalUpdateUI(wxUpdateUIEvent& event);
	void OnViewDisplaySimplifyUpdateUI(wxUpdateUIEvent& event);
	void OnViewDisplayTraditionalUpdateUI(wxUpdateUIEvent& event);
	void OnNextFile(wxCommandEvent& event);
	void OnPrevFile(wxCommandEvent& event);
	void OnRecentFile(wxCommandEvent& event);
	void OnBookMarks(wxCommandEvent& event);
	void OnAddBookMark(wxCommandEvent& event);
	void OnClearBookMarks(wxCommandEvent& event);
	void OnFullScreen(wxCommandEvent& event);
	void OnEncoding(wxCommandEvent& event);
	void OnEncodingUpdateUI(wxUpdateUIEvent& event);
	void OnHide(wxCommandEvent& event);
	void OnHotKeyShowWindow(wxKeyEvent& event);
	void OnLang(wxCommandEvent& event);
	void OnLangUpdateUI(wxUpdateUIEvent& event);

    virtual wxToolBar* OnCreateToolBar(long style,
                                        wxWindowID id,
                                        const wxString& name);
	void Init();
	void AddRecentFile(const wxString & strFileName);

	CReadBookCanvas * GetCanvas() const { return m_pCanvas; }

	wxMBConv * GetMBConv() const { return m_pMBConv; }

	void UpdateEncoding(const wxString & charset);

	void UpdateCharsetMenuItemText(const wxString & text);
	wxString GetCharsetMenuItemText();

	wxInt32 GetCurrentLang() const { return m_nLang; }

	DECLARE_EVENT_TABLE()

private:
	CReadBookCanvas * m_pCanvas;

	wxMenu * m_pRecentFileMenu;
	wxMenu * m_pEncodingMenu;
	wxMenuItem * m_pRecentFileMenuItem;

	wxUint32 m_nEncoding;
	wxUint32 m_nLang;

	wxMBConv * m_pMBConv;

	int m_nHotKeyId;

	CReadBookCanvas *CreateCanvas(wxView *view, wxFrame *parent);

	wxMenu * CreateRecentFilesMenu();

	void UpdateRecentFilesLabel();
};

#endif /*READBOOKMAINFRM_H_*/
