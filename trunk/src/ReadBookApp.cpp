#if _WIN32
#ifndef _WIN32_WCE
#if _DEBUG
//#include "vld.h"
#endif
#endif
#endif

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
#include "wx/wfstream.h"
#include "wx/stdpaths.h"
#include "wx/filesys.h"
#include "wx/fs_arc.h"
#include "wx/fs_filter.h"

#include "ReadBookApp.h"
#include "ReadBookMainFrm.h"
#include "ReadBookDocManager.h"

#include "presentation/ReadBookDoc.h"
#include "presentation/ReadBookView.h"

#include "presentation/buffered/ReadBookBufferedDoc.h"
#include "presentation/buffered/ReadBookBufferedView.h"

#include "presentation/simple/ReadBookSimpleDoc.h"
#include "presentation/simple/ReadBookSimpleView.h"
#include "presentation/simple/ObjectCache.h"

#include "presentation/textprocesslib/ReadBookTPLDoc.h"
#include "presentation/textprocesslib/ReadBookTPLView.h"

#include "presentation/textprocesslib/ReadBookTPLDoc2.h"
#include "presentation/textprocesslib/ReadBookTPLView2.h"

#ifndef __WXMSW__
#include "res/file.xpm"
#include "res/file32.xpm"
#include "res/folder.xpm"
#include "res/folder32.xpm"
#include "res/BOOK06.xpm"
#endif

CReadBookMainFrm * g_pMainFrm = NULL;

IMPLEMENT_APP(CReadBookApp)

CReadBookApp::CReadBookApp(void) {
	m_pDocManager = (wxDocManager *) NULL;
}

extern void Initialize7ZipClassFactories();
extern void Deinitialize7ZipClassFactories();

bool CReadBookApp::OnInit(void) {
	wxFileSystem::AddHandler(new wxArchiveFSHandler());
	wxFileSystem::AddHandler(new wxFilterFSHandler);
	Initialize7ZipClassFactories();

	LoadPreference();


	//// Create a document manager
	m_pDocManager = new CReadBookDocManager;


	//// Create a template relating drawing documents to their views
	wxString archiveExts = GetPreference()->GetArchiveFileFilters();

	if (archiveExts.Len() > 0)
		archiveExts += wxT(";");

	archiveExts += wxT("*.zip;*.tar;*.gz");


#ifdef _WIN32
#ifdef _WIN32_WCE
	//#define DOC_CLASS CReadBookSimpleDoc
	//#define VIEW_CLASS CReadBookSimpleView
#define DOC_CLASS CReadBookTPLDoc2
#define VIEW_CLASS CReadBookTPLView2
#else
#define DOC_CLASS CReadBookTPLDoc2
#define VIEW_CLASS CReadBookTPLView2
#endif
#else
#define DOC_CLASS CReadBookTPLDoc2
#define VIEW_CLASS CReadBookTPLView2
#endif
	(void) new wxDocTemplate(m_pDocManager, wxT("ReadBook Files"),
			wxT("*.txt;*.html;*.htm;") + archiveExts, wxT(""), wxT("rbk"),
			wxT("ReadBook Doc"), wxT("ReadBook View"), CLASSINFO(DOC_CLASS),
			CLASSINFO(VIEW_CLASS));

	(void) new wxDocTemplate(m_pDocManager, wxT("ReadBook Text Files"),
			wxT("*.txt;*.html;*.htm"), wxT(""), wxT("rbk"), wxT("ReadBook Doc"),
			wxT("ReadBook View"), CLASSINFO(DOC_CLASS), CLASSINFO(VIEW_CLASS));

	(void) new wxDocTemplate(m_pDocManager, wxT("ReadBook Archive Files"),
			archiveExts, wxT(""), wxT("rbk"), wxT("ReadBook Doc"),
			wxT("ReadBook View"), CLASSINFO(DOC_CLASS), CLASSINFO(VIEW_CLASS));

	(void) new wxDocTemplate(m_pDocManager, wxT("All Files"), wxT("*.*"),
			wxT(""), wxT(""), wxT("ReadBook Doc"), wxT("ReadBook View"),
			CLASSINFO(DOC_CLASS), CLASSINFO(VIEW_CLASS));


#ifdef __WXMAC__
	wxFileName::MacRegisterDefaultTypeAndCreator( wxT("rbk") , 'WXMB' , 'WXMA' );
#endif

	m_pDocManager->SetMaxDocsOpen(1);

	InitObjectCache();


	//// Create the main g_pMainFrm window
	g_pMainFrm = new CReadBookMainFrm(m_pDocManager, (wxFrame *) NULL, wxID_ANY,
			wxT("ReadBook wxWidget"), wxPoint(0, 0), wxSize(500, 400),
			wxDEFAULT_FRAME_STYLE);


	//// Give it an icon (this is ignored in MDI mode: uses resources)
	g_pMainFrm->SetIcon(wxICON(BOOK06));

	g_pMainFrm->Init();
	g_pMainFrm->Centre(wxBOTH);
	g_pMainFrm->Show(true);

	SetTopWindow(g_pMainFrm);

	g_pMainFrm->Maximize(true);

	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_NEW);

	g_pMainFrm->GetEventHandler()->ProcessEvent(event);

	return true;
}

int CReadBookApp::OnExit(void) {
	ReleaseObjectCache();

	delete m_pDocManager;
	SavePreference();

	Deinitialize7ZipClassFactories();

	return 0;
}

void CReadBookApp::LoadPreference() {
	const wxString & strFileName = GetConfigFileName();

	if (wxFileName::FileExists(strFileName) && wxFileName::IsFileReadable(
			strFileName)) {
		wxFileInputStream input(strFileName);

		m_ReadBookPreference.Load(input);
	}
}

void CReadBookApp::SavePreference() {
	const wxString & strFileName = GetConfigFileName();

	if (wxFileName::FileExists(strFileName) && !wxFileName::IsFileWritable(
			strFileName)) {
		return;
	}

	wxFileOutputStream output(strFileName);

	m_ReadBookPreference.Store(output);
}

const wxString CReadBookApp::GetConfigFileName() {
	wxStandardPaths stdPath;

	wxString dataDir = stdPath.GetUserDataDir();

	if (!::wxDirExists(dataDir)) {
		wxFileName::Mkdir(dataDir, 0777, wxPATH_MKDIR_FULL);
	}

	wxFileName filename(dataDir, wxT("ReadBook.dat"));

	return filename.GetFullPath();
}

void CReadBookApp::OpenRelatedFile(const wxString & fileName, wxInt16 nDelta) {
	if (fileName.Length() == 0)
		return;

	wxFileSystem fs;

	fs.ChangePathTo(fileName);

	wxFileName fn(fileName);

	wxArrayString files;

	wxString name = fs.FindFirst(wxT("*"));

	wxString currentName = fileName;

	while (name.Length() > 0) {
		wxFileName fn1(name);

		if (CanHandleFile(fn1)) {
			files.Add(name);

			if (fn1.GetFullPath().CmpNoCase(fn.GetFullPath()) == 0) {
				currentName = name;
			}
		}

		name = fs.FindNext();
	}

	files.Sort(false);

	wxInt16 index = files.Index(currentName, true, false);

	if (index >= 0) {
		index += nDelta;

		if (index >= 0 && (size_t) index < files.Count()) {
			m_pDocManager->CreateDocument(files.Item(index), wxDOC_SILENT);
		}
	}
}
