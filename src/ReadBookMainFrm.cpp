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

#include "wx/docview.h"
#include "wx/filename.h"
#include "wx/dir.h"
#include "wx/fontdlg.h"
#include "wx/filesys.h"
#include "wx/aboutdlg.h"
#include "wx/statline.h"
#include "wx/generic/aboutdlgg.h"

#include "ReadBookApp.h"
#include "ReadBookMainFrm.h"
#include "ReadBookCanvas.h"
#include "presentation/ReadBookDoc.h"
#include "presentation/ReadBookView.h"
#include "ReadBookPreferenceDlg.h"
#include "version.h"

const wxInt32 RECENT_FILE_SIZE = 20;

IMPLEMENT_CLASS(CReadBookMainFrm, wxDocParentFrame)

BEGIN_EVENT_TABLE(CReadBookMainFrm, wxDocParentFrame)
    EVT_MENU(IDM_ABOUT, CReadBookMainFrm::OnAbout)
    EVT_MENU(IDM_PREFERENCE, CReadBookMainFrm::OnPreference)
    EVT_MENU(IDM_GOTO, CReadBookMainFrm::OnGoto)
    EVT_MENU(IDM_VIEW_AS_HTML, CReadBookMainFrm::OnViewAsHtml)
    EVT_MENU(IDM_VIEW_AS_TEXT, CReadBookMainFrm::OnViewAsText)
    EVT_MENU(IDM_VIEW_DISPLAY_ORIGINAL, CReadBookMainFrm::OnViewDisplayOriginal)
    EVT_MENU(IDM_VIEW_DISPLAY_SIMPLIFY, CReadBookMainFrm::OnViewDisplaySimplify)
    EVT_MENU(IDM_VIEW_DISPLAY_TRADITIONAL, CReadBookMainFrm::OnViewDisplayTraditional)
    EVT_UPDATE_UI(IDM_VIEW_AS_HTML, CReadBookMainFrm::OnViewAsHtmlUpdateUI)
    EVT_UPDATE_UI(IDM_VIEW_AS_TEXT, CReadBookMainFrm::OnViewAsTextUpdateUI)
    EVT_UPDATE_UI(IDM_VIEW_DISPLAY_ORIGINAL, CReadBookMainFrm::OnViewDisplayOriginalUpdateUI)
    EVT_UPDATE_UI(IDM_VIEW_DISPLAY_SIMPLIFY, CReadBookMainFrm::OnViewDisplaySimplifyUpdateUI)
    EVT_UPDATE_UI(IDM_VIEW_DISPLAY_TRADITIONAL, CReadBookMainFrm::OnViewDisplayTraditionalUpdateUI)
    EVT_MENU(IDM_NEXT_FILE, CReadBookMainFrm::OnNextFile)
    EVT_MENU(IDM_PREV_FILE, CReadBookMainFrm::OnPrevFile)
    EVT_MENU(IDM_BOOKMARKS, CReadBookMainFrm::OnBookMarks)
    EVT_MENU(IDM_ADD_BOOKMARK, CReadBookMainFrm::OnAddBookMark)
    EVT_MENU(IDM_CLEAR_BOOKMARKS, CReadBookMainFrm::OnClearBookMarks)
    EVT_MENU(IDM_HIDE, CReadBookMainFrm::OnHide)
    EVT_MENU(IDM_FULL_SCREEN, CReadBookMainFrm::OnFullScreen)
    EVT_MENU_RANGE(IDM_ENCODE_ID_START, IDM_ENCODE_ID_END, CReadBookMainFrm::OnEncoding)
    EVT_UPDATE_UI_RANGE(IDM_ENCODE_ID_START, IDM_ENCODE_ID_END, CReadBookMainFrm::OnEncodingUpdateUI)
    EVT_MENU_RANGE(IDM_RECENT_FILE, IDM_RECENT_FILE_END, CReadBookMainFrm::OnRecentFile)
END_EVENT_TABLE()

CReadBookMainFrm::CReadBookMainFrm(wxDocManager *manager,
                wxFrame *frame, wxWindowID id, const wxString& title,
                const wxPoint& pos, const wxSize& size, const long type):
wxDocParentFrame(manager, frame, id, title, pos, size, type),
m_pCanvas(NULL),
m_pRecentFileMenu(NULL),
m_nEncoding(0),
m_pMBConv(NULL)
{
	UpdateEncoding(IDM_ENCODE_GB);
}

CReadBookMainFrm::~CReadBookMainFrm()
{
#ifdef __WXMSW__
    UnregisterHotKey(m_nHotKeyId);
    ::GlobalDeleteAtom(m_nHotKeyId);
#endif

	if (m_pMBConv != NULL)
	{
		delete m_pMBConv;
	}
}

class MyAboutDialog : public wxGenericAboutDialog
{
public:
    MyAboutDialog(const wxAboutDialogInfo& info)
    {
        Create(info);
    }

    // add some custom controls
    virtual void DoAddCustomControls()
    {
        AddControl(new wxStaticLine(this), wxSizerFlags().Expand());
		wxString strAbout = wxT("Keyboard Short Cut:\n");
		strAbout += wxT("\tF11  Toggle FullScreenView\n\n");
		strAbout += wxT("Windows Only Keyboard Short Cut:\n");
		strAbout += wxT("\tESCAPE Boss Key Hide the Main Frame\n");
		strAbout += wxT("\tCTRL+ALT+S Restore the Hidden Main Frame\n");
        AddText(strAbout);
        AddControl(new wxStaticLine(this), wxSizerFlags().Expand());
    }
};

void CReadBookMainFrm::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
	wxAboutDialogInfo info;
    info.SetName(_T("wxReadBook"));
    info.SetVersion(VERSION_NUM_DOT_STRING_T);
    info.SetDescription(_T("a Book reader created using wxWidget"));
    info.SetCopyright(_T("(C) 2008-2009 Angel & Stone"));
    info.AddDeveloper(_T("Jingnan Si"));
    info.SetWebSite(_T("http://code.google.com/p/wxreadbook/"), 
		_T("Support site: http://code.google.com/p/wxreadbook/"));

	MyAboutDialog dlg(info);

	dlg.ShowModal();
}

CReadBookCanvas * CReadBookMainFrm::CreateCanvas(wxView *view, wxFrame *parent)
{
    int width, height;
    parent->GetClientSize(&width, &height);

    // Non-retained m_pCanvas
    CReadBookCanvas * pCanvas = new CReadBookCanvas(view,
        parent,
        wxPoint(0, 0),
        wxSize(width, height), 0);
    //pCanvas->SetCursor(wxCursor(wxCURSOR_PENCIL));

    // Give it scrollbars
    //pCanvas->SetBackgroundColour(*wxWHITE);
    pCanvas->SetBackgroundColour(wxGetApp().GetPreference()->GetBackgroundColor());
    pCanvas->ClearBackground();

    return pCanvas;
}

void CReadBookMainFrm::Init()
{
    m_pCanvas = CreateCanvas(NULL, this);

    //// Make a menubar
    wxMenu *pFileMenu = new wxMenu;

    //pFileMenu->Append(wxID_NEW, wxT("&New..."));
    pFileMenu->Append(wxID_OPEN, wxT("&Open...\tCTRL+O"));

    pFileMenu->Append(wxID_CLOSE, wxT("&Close"));
    //pFileMenu->Append(wxID_SAVE, wxT("&Save"));
    //pFileMenu->Append(wxID_SAVEAS, wxT("Save &As..."));
    //pFileMenu->AppendSeparator();
    //pFileMenu->Append(wxID_PRINT, wxT("&Print..."));
    //pFileMenu->Append(wxID_PRINT_SETUP, wxT("Print &Setup..."));
    //pFileMenu->Append(wxID_PREVIEW, wxT("Print Pre&view"));

    pFileMenu->AppendSeparator();

    m_pRecentFileMenu = CreateRecentFilesMenu();

    if (m_pRecentFileMenu != NULL)
    {
        m_pRecentFileMenuItem = pFileMenu->AppendSubMenu(m_pRecentFileMenu, wxT("Recent Files"));
    }
    else
    {
        m_pRecentFileMenu = new wxMenu();

        m_pRecentFileMenuItem = pFileMenu->AppendSubMenu(m_pRecentFileMenu, wxT("Recent Files"));

        m_pRecentFileMenuItem->Enable(false);
    }

#ifdef __WXMSW__
    pFileMenu->AppendSeparator();
    pFileMenu->Append(IDM_HIDE, wxT("&Hide\tESCAPE"));
#endif

    pFileMenu->AppendSeparator();
    pFileMenu->Append(wxID_EXIT, wxT("E&xit\tCTRL+X"));

    // A nice touch: a history of files visited. Use this menu.
    //GetDocumentManager()->FileHistoryUseMenu(pFileMenu);

    wxMenu * pEditMenu = new wxMenu;
    //pEditMenu->Append(wxID_UNDO, wxT("&Undo"));
    //pEditMenu->Append(wxID_REDO, wxT("&Redo"));
    //pEditMenu->Append(wxID_COPY, wxT("&Copy"));
    //pEditMenu->Append(wxID_CUT, wxT("C&ut"));
    //pEditMenu->Append(wxID_PASTE, wxT("&Paste"));
    //pEditMenu->AppendSeparator();
    pEditMenu->Append(IDM_PREFERENCE, wxT("&Preference..."));
    pEditMenu->AppendSeparator();
    pEditMenu->Append(IDM_GOTO, wxT("&Goto...\tCTRL+G"));

	wxMenu * pEncodingMenu = new wxMenu;
    pEncodingMenu->AppendCheckItem(IDM_ENCODE_GB, wxT("GB2312/GB18030"));
    pEncodingMenu->AppendCheckItem(IDM_ENCODE_BIG5, wxT("Big5"));
	pEncodingMenu->AppendCheckItem(IDM_ENCODE_UTF7, wxT("UTF7"));
    pEncodingMenu->AppendCheckItem(IDM_ENCODE_UTF8, wxT("UTF8"));
    pEncodingMenu->AppendCheckItem(IDM_ENCODE_UNICODE, wxT("Unicode"));
    pEncodingMenu->AppendCheckItem(IDM_ENCODE_UNICODE_BE, wxT("Unicode Big Endian"));
    pEncodingMenu->AppendCheckItem(IDM_ENCODE_UNICODE_32, wxT("Unicode 32"));
    pEncodingMenu->AppendCheckItem(IDM_ENCODE_UNICODE_32_BE, wxT("Unicode 32 Big Endian"));

    wxMenu * pViewMenu = new wxMenu;
    pViewMenu->AppendCheckItem(IDM_VIEW_AS_HTML, wxT("View as &Html"));
    pViewMenu->AppendCheckItem(IDM_VIEW_AS_TEXT, wxT("View as &Text"));
    pViewMenu->AppendSeparator();
    pViewMenu->Append(IDM_NEXT_FILE, wxT("Next File"));
    pViewMenu->Append(IDM_PREV_FILE, wxT("Previous File"));

	wxMenu * pDisplayMenu = new wxMenu;

	wxChar original[] = {21407,22987};
	wxChar simplify[] = {31616,20307};
	wxChar traditional[] = {32321,39636};

	pDisplayMenu->AppendCheckItem(IDM_VIEW_DISPLAY_ORIGINAL, wxString(original, 2));
    pDisplayMenu->AppendCheckItem(IDM_VIEW_DISPLAY_SIMPLIFY, wxString(simplify, 2));
    pDisplayMenu->AppendCheckItem(IDM_VIEW_DISPLAY_TRADITIONAL, wxString(traditional, 2));

#if wxUSE_UNICODE
	pViewMenu->AppendSeparator();
    pViewMenu->AppendSubMenu(pEncodingMenu, wxT("Encoding..."));

	pViewMenu->AppendSeparator();
	pViewMenu->AppendSubMenu(pDisplayMenu, wxT("Display as..."));
#endif
    pViewMenu->AppendSeparator();
    pViewMenu->Append(IDM_ADD_BOOKMARK, wxT("&Add Bookmark...\tCTRL+B"));
    pViewMenu->Append(IDM_CLEAR_BOOKMARKS, wxT("&Clear Bookmarks"));
    pViewMenu->Append(IDM_BOOKMARKS, wxT("Bookmarks..."));

#ifndef _WIN32_WCE
    pViewMenu->AppendSeparator();
    pViewMenu->Append(IDM_FULL_SCREEN, wxT("FullScreen\tF11"));
#endif

    wxMenu *pHelpMenu = new wxMenu;
    pHelpMenu->Append(IDM_ABOUT, wxT("&About"));

    wxMenuBar *pMenuBar = new wxMenuBar;

    pMenuBar->Append(pFileMenu, wxT("&File"));
    pMenuBar->Append(pEditMenu, wxT("&Edit"));
    pMenuBar->Append(pViewMenu, wxT("&View"));
    pMenuBar->Append(pHelpMenu, wxT("&Help"));

    //// Associate the menu bar with the g_pMainFrm
    SetMenuBar(pMenuBar);

#ifdef _WIN32_WCE
    CreateToolBar();
#endif

    //Register System wide hot key

#ifdef __WXMSW__
    m_nHotKeyId = ::GlobalAddAtom(wxT("wxReadBookHotKey_ShowWindow"));
    RegisterHotKey(m_nHotKeyId, wxMOD_CONTROL | wxMOD_ALT, 'S');
    Connect(m_nHotKeyId, wxEVT_HOTKEY, wxCharEventHandler(CReadBookMainFrm::OnHotKeyShowWindow));
#endif
	m_pCanvas->SetFocus();
}

void CReadBookMainFrm::OnPreference(wxCommandEvent& WXUNUSED(event))
{
	CReadBookPreferenceDlg dlg(this);

	if (dlg.ShowModal() == wxID_OK)
	{
		wxGetApp().GetPreference()->SetFont(dlg.GetFont());
		wxGetApp().GetPreference()->SetTextColor(dlg.GetTextColor());
		wxGetApp().GetPreference()->SetBackgroundColor(dlg.GetBackgroundColor());
		wxGetApp().GetPreference()->SetLineMargin(dlg.GetLineMargin());
		wxGetApp().GetPreference()->SetColumnMargin(dlg.GetColumnMargin());

		if (m_pCanvas)
		{
			m_pCanvas->SetBackgroundColour(wxGetApp().GetPreference()->GetBackgroundColor());

			if (m_pCanvas->GetView())
			{
				CReadBookView * pView = (CReadBookView *)(m_pCanvas->GetView());

				pView->PreferenceChanged();
			}

			m_pCanvas->Refresh();
		}
	}
}

void CReadBookMainFrm::OnViewAsHtml(wxCommandEvent& WXUNUSED(event))
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = (CReadBookView *)(m_pCanvas->GetView());

		pView->SetViewMode(wxReadBook::Html);
	}
}

void CReadBookMainFrm::OnViewAsText(wxCommandEvent& WXUNUSED(event))
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		pView->SetViewMode(wxReadBook::Text);
	}
}

void CReadBookMainFrm::OnViewAsHtmlUpdateUI(wxUpdateUIEvent& event)
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		event.Check(pView->GetViewMode() == wxReadBook::Html);
	}
}

void CReadBookMainFrm::OnViewAsTextUpdateUI(wxUpdateUIEvent& event)
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		event.Check(pView->GetViewMode() == wxReadBook::Text);
	}
}

void CReadBookMainFrm::OnNextFile(wxCommandEvent& WXUNUSED(event))
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		CReadBookDoc * pDoc = pView->GetReadBookDoc();

		wxGetApp().OpenRelatedFile(pDoc->GetFileName(), 1);
	}
}

void CReadBookMainFrm::OnPrevFile(wxCommandEvent& WXUNUSED(event))
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		CReadBookDoc * pDoc = pView->GetReadBookDoc();

		wxGetApp().OpenRelatedFile(pDoc->GetFileName(), -1);
	}
}

wxMenu * CReadBookMainFrm::CreateRecentFilesMenu()
{
    const CFileInfoList * pInfoList = wxGetApp().GetPreference()->GetFileInfoMap();

    if (pInfoList->size() == 0)
    {
        return NULL;
    }

    wxMenu * pMenu = new wxMenu();

    wxFileSystem fs;

    CFileInfoList::const_iterator it;

    int count = 0;

	for(it = pInfoList->begin();it != pInfoList->end(); ++it)
	{
		CFileInfo * pInfo = *it;

        wxFSFile * pFile = fs.OpenFile(pInfo->m_strFileName);

        if (pFile != NULL)
        {
            wxString itemName = wxT("");

			wxString location = pFile->GetLocation();

            itemName.Printf(wxT("%d. %s"), count + 1, pInfo->m_strFileName.c_str());

            wxMenuItem * pItem =
                pMenu->Append(IDM_RECENT_FILE + count, itemName);

            pItem->SetHelp(pInfo->m_strFileName);

            count++;

            if (count == RECENT_FILE_SIZE)
            {
                delete pFile;
                break;
            }

            delete pFile;
        }
	}

	return pMenu;
}

void CReadBookMainFrm::OnRecentFile(wxCommandEvent& event)
{
    wxMenuItem * pItem = m_pRecentFileMenu->FindItem(event.GetId(),NULL);

    if (pItem != NULL)
    {
        wxGetApp().GetDocManager()->CreateDocument(pItem->GetHelp(), wxDOC_SILENT);
    }
}

void CReadBookMainFrm::AddRecentFile(const wxString & strFileName)
{
    if (strFileName.IsEmpty())
        return;

    m_pRecentFileMenuItem->Enable(true);

    size_t count = m_pRecentFileMenu->GetMenuItemCount();

    for(size_t i = 0; i < count; i++)
    {
        wxMenuItem * pItem = m_pRecentFileMenu->FindItemByPosition(i);

        if (pItem->GetHelp().CmpNoCase(strFileName) == 0)
        {
            m_pRecentFileMenu->Remove(pItem);
            m_pRecentFileMenu->Prepend(pItem);

            UpdateRecentFilesLabel();
            return;
        }
    }

    wxString itemName = wxT("");
    itemName.Printf(wxT("%d. %s"), count, strFileName.c_str());

    wxMenuItem * pNewItem =
        m_pRecentFileMenu->Prepend(count + IDM_RECENT_FILE,
            itemName);

    pNewItem->SetHelp(strFileName);

    if (m_pRecentFileMenu->GetMenuItemCount() > (size_t)RECENT_FILE_SIZE)
	{
        wxMenuItem * pRemovedItem =
			m_pRecentFileMenu->Remove(m_pRecentFileMenu->FindItemByPosition(RECENT_FILE_SIZE));

		delete pRemovedItem;
	}

    UpdateRecentFilesLabel();
}

void CReadBookMainFrm::UpdateRecentFilesLabel()
{
    size_t count = m_pRecentFileMenu->GetMenuItemCount();

    for(size_t i = 0; i < count; i++)
    {
        wxMenuItem * pItem = m_pRecentFileMenu->FindItemByPosition(i);

        wxString itemName = wxT("");
        itemName.Printf(wxT("%d. %s"), i + 1, pItem->GetHelp().c_str());

        pItem->SetItemLabel(itemName);
    }

    m_pRecentFileMenu->UpdateUI();
}

void CReadBookMainFrm::OnFullScreen(wxCommandEvent& event)
{
    this->ShowFullScreen(!this->IsFullScreen());
}

void CReadBookMainFrm::OnHide(wxCommandEvent& event)
{
    if (IsShown())
        Hide();
}

void CReadBookMainFrm::OnHotKeyShowWindow(wxKeyEvent& event)
{
    if (!IsShown())
    {
        Raise();
        Show();
    }
}

void CReadBookMainFrm::OnEncoding(wxCommandEvent& event)
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		CReadBookDoc * pDoc = pView->GetReadBookDoc();

		if (m_nEncoding == event.GetId() && m_pMBConv != NULL)
		{
			pDoc->ReloadByEncoding(m_pMBConv);

			return;
		}

		UpdateEncoding(event.GetId());

		if (m_pMBConv != NULL)
			pDoc->ReloadByEncoding(m_pMBConv);
	}
}

void CReadBookMainFrm::OnEncodingUpdateUI(wxUpdateUIEvent& event)
{
	event.Check(m_nEncoding == event.GetId());
}

#if !defined(_WIN32) || !defined(WINDOWSCE)
extern wxMBConv* new_wxMBConv_iconv( const wxChar* name );
#endif

void CReadBookMainFrm::UpdateEncoding(wxUint32 id)
{
	if (m_nEncoding == id && m_pMBConv != NULL)
		return;

	if (m_pMBConv != NULL)
		delete m_pMBConv;

	m_nEncoding = id;

	switch(id)
	{
	case IDM_ENCODE_WINDOWS_1252:
		{
#if defined(_WIN32) || defined(WINDOWSCE)
			m_pMBConv = new wxCSConv(wxFONTENCODING_CP1252);
#else
			m_pMBConv = new_wxMBConv_iconv(wxT("WINDOWS_1253"));
#endif
			break;
		}
	case IDM_ENCODE_GB:
		{
#if defined(_WIN32) || defined(WINDOWSCE)
			m_pMBConv = new wxCSConv(wxFONTENCODING_CP936);
#else
			m_pMBConv = new_wxMBConv_iconv(wxT("GB18030"));
#endif
			break;
		}
	case IDM_ENCODE_BIG5:
		{
#if defined(_WIN32) || defined(WINDOWSCE)
			m_pMBConv = new wxCSConv(wxFONTENCODING_CP950);
#else
			m_pMBConv = new_wxMBConv_iconv(wxT("BIG5"));
#endif
			break;
		}
	case IDM_ENCODE_UTF7:
		{
			m_pMBConv = new wxMBConvUTF7;
			break;
		}
	case IDM_ENCODE_UTF8:
		{
			m_pMBConv = new wxMBConvUTF8;
			break;
		}
	case IDM_ENCODE_UNICODE:
		{
			m_pMBConv = new wxMBConvUTF16LE;
			break;
		}
	case IDM_ENCODE_UNICODE_BE:
		{
			m_pMBConv = new wxMBConvUTF16BE;
			break;
		}
	case IDM_ENCODE_UNICODE_32:
		{
			m_pMBConv = new wxMBConvUTF32LE;
			break;
		}
	case IDM_ENCODE_UNICODE_32_BE:
		{
			m_pMBConv = new wxMBConvUTF32BE;
			break;
		}
	default:
		break;
	}
}

void CReadBookMainFrm::OnBookMarks(wxCommandEvent& event)
{
}

void CReadBookMainFrm::OnAddBookMark(wxCommandEvent& event)
{
}

void CReadBookMainFrm::OnClearBookMarks(wxCommandEvent& event)
{
}

void CReadBookMainFrm::OnViewDisplayOriginal(wxCommandEvent& WXUNUSED(event))
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		pView->SetDisplayAs(wxReadBook::DisplayAsOriginal);
	}
}

void CReadBookMainFrm::OnViewDisplaySimplify(wxCommandEvent& WXUNUSED(event))
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		pView->SetDisplayAs(wxReadBook::DisplayAsSimplify);
	}
}

void CReadBookMainFrm::OnViewDisplayTraditional(wxCommandEvent& WXUNUSED(event))
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		pView->SetDisplayAs(wxReadBook::DisplayAsTraditional);
	}
}

void CReadBookMainFrm::OnViewDisplayOriginalUpdateUI(wxUpdateUIEvent& event)
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		event.Check(pView->GetDisplayAs() == wxReadBook::DisplayAsOriginal);
	}
}

void CReadBookMainFrm::OnViewDisplaySimplifyUpdateUI(wxUpdateUIEvent& event)
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		event.Check(pView->GetDisplayAs() == wxReadBook::DisplayAsSimplify);
	}
}
void CReadBookMainFrm::OnViewDisplayTraditionalUpdateUI(wxUpdateUIEvent& event)
{
	if (m_pCanvas && m_pCanvas->GetView())
	{
		CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

		event.Check(pView->GetDisplayAs() == wxReadBook::DisplayAsTraditional);
	}
}

wxToolBar* CReadBookMainFrm::OnCreateToolBar(long style,
                                        wxWindowID id,
                                        const wxString& name)
{
#if defined(__WXWINCE__) && defined(__POCKETPC__)
    return new wxToolMenuBar(this, id,
                         wxDefaultPosition, wxDefaultSize,
                         style, name, GetMenuBar());
#else
    return new wxToolBar(this, id,
                         wxDefaultPosition, wxDefaultSize,
                         style, name);
#endif
}

void CReadBookMainFrm::OnGoto(wxCommandEvent& WXUNUSED(event))
{
	CReadBookView * pView = ((CReadBookView *)m_pCanvas->GetView());

	pView->GoTo();
}
