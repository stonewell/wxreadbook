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

    virtual wxToolBar* OnCreateToolBar(long style,
                                        wxWindowID id,
                                        const wxString& name);
	void Init();
	void AddRecentFile(const wxString & strFileName);

	CReadBookCanvas * GetCanvas() const { return m_pCanvas; }

	wxMBConv * GetMBConv() const { return m_pMBConv; }

	void UpdateEncoding(wxUint32 id);

	DECLARE_EVENT_TABLE()

private:
	CReadBookCanvas * m_pCanvas;

	wxMenu * m_pRecentFileMenu;
	wxMenuItem * m_pRecentFileMenuItem;

	wxUint32 m_nEncoding;

	wxMBConv * m_pMBConv;

	int m_nHotKeyId;

	CReadBookCanvas *CreateCanvas(wxView *view, wxFrame *parent);

	wxMenu * CreateRecentFilesMenu();

	void UpdateRecentFilesLabel();
};

enum MenuIDEnum
{
	MENU_ID_START = wxID_HIGHEST + 1,

	IDM_ABOUT = MENU_ID_START,
	IDM_PREFERENCE,
	IDM_VIEW_AS_HTML,
	IDM_VIEW_AS_TEXT,
	IDM_NEXT_FILE,
	IDM_PREV_FILE,
	IDM_BOOKMARKS,
	IDM_ADD_BOOKMARK,
	IDM_CLEAR_BOOKMARKS, 
	IDM_HIDE,
	IDM_FULL_SCREEN,
	IDM_VIEW_DISPLAY_ORIGINAL,
	IDM_VIEW_DISPLAY_SIMPLIFY,
	IDM_VIEW_DISPLAY_TRADITIONAL,
	IDM_GOTO,

	IDM_ENCODE_UNKNOWN,
	IDM_ENCODE_ID_START,
	IDM_ENCODE_GB = IDM_ENCODE_ID_START,
	IDM_ENCODE_BIG5,
	IDM_ENCODE_UTF7,
	IDM_ENCODE_UTF8,
	IDM_ENCODE_UNICODE,
	IDM_ENCODE_UNICODE_BE,
	IDM_ENCODE_UNICODE_32,
	IDM_ENCODE_UNICODE_32_BE,
	IDM_ENCODE_ID_END = IDM_ENCODE_UNICODE_32_BE,

	IDM_RECENT_FILE,
	IDM_RECENT_FILE_END = IDM_RECENT_FILE + 20,
};


#endif /*READBOOKMAINFRM_H_*/
