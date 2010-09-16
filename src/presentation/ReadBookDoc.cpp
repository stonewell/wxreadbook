// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/dir.h"
#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "wx/filesys.h"
#include "wx/fs_arc.h"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "../ReadBookApp.h"
#include "../unicode/UnicodeHelper.h"
#include "ReadBookDoc.h"
#include "ReadBookView.h"

const wxChar EMPTY_STRING_CHARS[] = { '\xA1', '\t', ' ', '\n', '\r', 0x00 };

const wxString EMPTY_STRING = wxEmptyString;
IMPLEMENT_DYNAMIC_CLASS(CReadBookDoc, wxDocument)

CReadBookDoc::CReadBookDoc() :
	m_nCurrentLine(0) {
}

CReadBookDoc::~CReadBookDoc() {
}

// Since text windows have their own method for saving to/loading from files,
// we override OnSave/OpenDocument instead of Save/LoadObject
bool CReadBookDoc::OnSaveDocument(const wxString& WXUNUSED(filename)) {
	Modify(false);


#ifdef __WXMAC__
	wxFileName fn(filename);
	fn.MacSetDefaultTypeAndCreator();
#endif

	return true;
}

bool CReadBookDoc::OnOpenDocument(const wxString& filename) {
	return OpenDocument(filename, GetCurrentMBConv(), true);
}

bool CReadBookDoc::IsModified(void) const {
	return wxDocument::IsModified();
}

void CReadBookDoc::Modify(bool mod) {
	wxDocument::Modify(mod);
}

const wxString & CReadBookDoc::GetLine(wxInt32 nRow) {
	if (nRow >= static_cast<wxInt32> (m_Buffer.GetCount())) {
		return EMPTY_STRING;
	}

	return m_Buffer.Item(nRow);
}

wxFileOffset CReadBookDoc::GetBufferSize(void) const {
	return m_Buffer.GetCount();
}

void CReadBookDoc::SetCurrentLine(wxInt32 nCurrentLine) {
	m_nCurrentLine = nCurrentLine;

	::wxGetApp().GetPreference()->SetFileInfo(m_strFileName, m_nCurrentLine,
			GetRowOffset(m_nCurrentLine));
}

bool CReadBookDoc::IsEmptyLine(const wxString & line) const {
	if (line.Length() == 0)
		return true;

	if (line.find_first_not_of(EMPTY_STRING_CHARS) == wxString::npos)
		return true;

	return false;
}

bool CReadBookDoc::OnNewDocument() {
	if (!wxDocument::OnNewDocument())
		return false;

	m_Buffer.Clear();

	m_strFileName = wxGetApp().GetPreference()->GetLastFile();

	if (m_strFileName.Length() > 0) {
		return OnOpenDocument(m_strFileName);
	}

	return true;
}

bool CReadBookDoc::OnCreate(const wxString& path, long flags) {
	if (!wxDocument::OnCreate(path, flags))
		return false;

	return true;
}

bool CReadBookDoc::OnCloseDocument() {
	return wxDocument::OnCloseDocument();
}

bool CReadBookDoc::OpenDocument(const wxString & filename, wxMBConv * conv,
		bool bGuess) {
	m_Buffer.Clear();

	bool isDir = true;

	wxString url = FileNameToUrl(filename, isDir);

	if (isDir) {
		wxString archiveFile;

		if (ChooseArchiveFile(url, archiveFile)) {
			url = FileNameToUrl(archiveFile, isDir);
		} else {
			return true;
		}
	}

	CFileInfo * pFileInfo = NULL;

	m_strFileName = wxFileSystem::URLToFileName(url).GetFullPath();

	m_nCurrentLine = wxGetApp().GetPreference()->GetFileInfo(m_strFileName,
			&pFileInfo);

	if (!LoadBuffer(url, conv, bGuess))
		return true;

	SetFilename(m_strFileName, true);

	if (pFileInfo == NULL) {
		m_nCurrentLine = 0;
		wxGetApp().GetPreference()->SetFileInfo(m_strFileName, m_nCurrentLine, 0);
	}

	Modify(false);
	UpdateAllViews();

	SetMainFrameTitle(m_strFileName);
	AddToRecentFile(m_strFileName);

	if (pFileInfo != NULL) {
		CReadBookView * pView = GetCurrentView();

		if (pView != NULL) {
			wxReadBook::DisplayAsEnum displayAs = pFileInfo->m_nDisplayAs;

			pView->SetViewMode(pFileInfo->m_nViewMode);


			//UpdateViewMode will replace display as, so use the saved display as
			pView->SetDisplayAs(displayAs);
		}
	}

	return true;
}

void CReadBookDoc::ReloadByEncoding(wxMBConv * conv) {
	OpenDocument(m_strFileName, conv, false);
}

void CReadBookDoc::ReloadCurrentDocument() {
	OpenDocument(m_strFileName, GetCurrentMBConv(), true);
}

bool CReadBookDoc::LoadBuffer(const wxString & url, wxMBConv * conv,
		bool bGuess) {
	wxFileSystem fs;

	fs.ChangePathTo(url, false);

	wxFSFile * pFile = fs.OpenFile(url, wxFS_READ | wxFS_SEEKABLE);

	if (pFile == NULL)
		return false;

	wxInputStream * pInput = pFile->GetStream();


#if wxUSE_UNICODE
	wxMBConv * pConv = GetSuitableMBConv(pInput, conv, bGuess);

	wxTextInputStream text(*pInput, wxT("\t"), *pConv);
#else
	wxTextInputStream text( *pInput );
#endif

	while (!pInput->Eof()) {
		wxString line = text.ReadLine();

		if (!IsEmptyLine(line))
			m_Buffer.Add(line);
	}

	delete pFile;

	return true;
}

wxMBConv * CReadBookDoc::GetSuitableMBConv(wxInputStream * pInput,
		wxMBConv * pDefaultConv, bool bGuess) {
	wxMBConv * pConv = pDefaultConv;
	
	if (bGuess) {
		wxFileOffset offset = pInput->TellI();

		wxString charset;

		if (GuessDataEncoding(pInput, charset, GetCurrentLang())) {
			UpdateCurrentEncoding(charset);
			pConv = GetCurrentMBConv();
		}

		pInput->SeekI(offset);
	}

	return pConv;
}

wxFileOffset CReadBookDoc::GetRowOffset(wxInt32 nRow) {
	return -1;
}

void CReadBookDoc::UpdateDisplay(wxReadBook::DisplayAsEnum displayAs,
		wxReadBook::ViewModeEnum viewMode) {
	CFileInfo * pFileInfo = NULL;

	::wxGetApp().GetPreference()->GetFileInfo(m_strFileName, &pFileInfo);

	::wxGetApp().GetPreference()->SetFileInfo(m_strFileName, m_nCurrentLine,
			pFileInfo == NULL ? GetRowOffset(m_nCurrentLine) : pFileInfo->m_nFilePos,
			displayAs, viewMode);
}
