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

#include "ReadBookApp.h"
#include "ReadBookMainFrm.h"
#include "ReadBookDoc.h"
#include "ReadBookView.h"
#include "ReadBookCanvas.h"
#include "ArchiveExplorerDlg.h"

#if wxUSE_UNICODE
#include "nsDetector.h"

class CObserver : public CnsICharsetDetectionObserver
{
public:
	bool m_bDone;
	String m_Charset;

	CObserver() :
		m_bDone(false),
		m_Charset(wxT(""))
	{
	}

	virtual void Notify(String charset) 
	{
		if (!m_bDone)
		{
			m_Charset = charset;
			m_bDone = true;
		}
	}
};
#endif

const wxChar EMPTY_STRING_CHARS[] = {'\xA1','\t',' ','\n','\r',0x00};

const wxString EMPTY_STRING = wxEmptyString;

IMPLEMENT_DYNAMIC_CLASS(CReadBookDoc, wxDocument)

CReadBookDoc::CReadBookDoc()  :
m_nCurrentLine(0)
{
}

CReadBookDoc::~CReadBookDoc()
{
}

// Since text windows have their own method for saving to/loading from files,
// we override OnSave/OpenDocument instead of Save/LoadObject
bool CReadBookDoc::OnSaveDocument(const wxString& WXUNUSED(filename))
{
	Modify(false);

#ifdef __WXMAC__
	wxFileName fn(filename) ;
	fn.MacSetDefaultTypeAndCreator() ;
#endif

	return true;
}

bool CReadBookDoc::OnOpenDocument(const wxString& filename)
{
	CReadBookMainFrm * pMainFrame = (CReadBookMainFrm *)(GetMainFrame());

	return OpenDocument(filename, pMainFrame->GetMBConv(), true);
}

bool CReadBookDoc::IsModified(void) const
{
	return wxDocument::IsModified();
}

void CReadBookDoc::Modify(bool mod)
{
	wxDocument::Modify(mod);
}

const wxString & CReadBookDoc::GetLine(wxInt32 nRow)
{
	if (nRow >= static_cast<wxInt32>(m_Buffer.GetCount()))
	{
		return EMPTY_STRING;
	}

	return m_Buffer.Item(nRow);
}

wxFileOffset CReadBookDoc::GetBufferSize(void) const
{
	return m_Buffer.GetCount();
}

void CReadBookDoc::SetCurrentLine(wxInt32 nCurrentLine)
{
	m_nCurrentLine = nCurrentLine;

	::wxGetApp().GetPreference()->SetFileInfo(m_strFileName, 
		m_nCurrentLine, 
		GetRowOffset(m_nCurrentLine));
}

bool CReadBookDoc::IsEmptyLine(const wxString & line) const
{
	if (line.Length() == 0)
		return true;

	if (line.find_first_not_of(EMPTY_STRING_CHARS) == wxString::npos)
		return true;

	return false;
}

bool CReadBookDoc::OnNewDocument()
{
	if (!wxDocument::OnNewDocument())
		return false;

	m_Buffer.Clear();

	m_strFileName = wxGetApp().GetPreference()->GetLastFile();

	if (m_strFileName.Length() > 0)
	{
		return OnOpenDocument(m_strFileName);
	}

	return true;
}

bool CReadBookDoc::OnCreate(const wxString& path, long flags)
{
	if (!wxDocument::OnCreate(path,flags))
		return false;

	return true;
}

bool CReadBookDoc::OnCloseDocument()
{
	return wxDocument::OnCloseDocument();
}

bool CReadBookDoc::OpenDocument(const wxString & filename, wxMBConv * conv, bool bGuess)
{
	m_Buffer.Clear();

	bool isDir = true;

	wxString url = FileNameToUrl(filename, isDir);

	if (isDir)
	{
		CArchiveExplorerDlg arDlg(GetMainFrame(),url);

		if (arDlg.ShowModal() == wxID_OK)
		{
			url = FileNameToUrl(arDlg.GetSelectedFilePath(), isDir);
		}
		else
		{
			return true;
		}
	}

	if (!LoadBuffer(url, conv, bGuess))
		return true;

	m_strFileName = url;

	SetFilename(m_strFileName, true);

	CFileInfo * pFileInfo = NULL;

	m_nCurrentLine = wxGetApp().GetPreference()->GetFileInfo(m_strFileName, &pFileInfo);

	if (pFileInfo == NULL)
	{
		wxGetApp().GetPreference()->SetFileInfo(m_strFileName, m_nCurrentLine, 0);
	}

	Modify(false);
	UpdateAllViews();

	CReadBookMainFrm * pMainFrame = (CReadBookMainFrm *)(GetMainFrame());
	pMainFrame->SetTitle(m_strFileName);
	pMainFrame->AddRecentFile(m_strFileName);

	if (pFileInfo != NULL)
	{
		CReadBookView * pView = (CReadBookView *)pMainFrame->GetCanvas()->GetView();

		if (pView != NULL)
		{
			wxReadBook::DisplayAsEnum displayAs = pFileInfo->m_nDisplayAs;

			pView->SetViewMode(pFileInfo->m_nViewMode);

			//UpdateViewMode will replace display as, so use the saved display as
			pView->SetDisplayAs(displayAs);
		}
	}

	return true;
}

void CReadBookDoc::ReloadByEncoding(wxMBConv * conv)
{
	OpenDocument(m_strFileName, conv, false);
}

wxUint32 CReadBookDoc::GuessDataEncoding(wxInputStream * pInput)
{
	CObserver ob;

	CnsDetector det(CnsPSMDetector::CHINESE);

	det.Init(&ob);

	char buf[1024] = {0};

	pInput->Read(buf, 1024);

	size_t count = pInput->LastRead();

	if (!det.isAscii(buf, count))
	{
		bool done = det.DoIt(buf, count, false);

		while(!done && !ob.m_bDone && !pInput->Eof())
		{
			pInput->Read(buf, 1024);

			count = pInput->LastRead();

			done = det.DoIt(buf, count, false);
		}
		det.DataEnd();

		StringVector ppCharsets;
		det.GetProbableCharsets(ppCharsets);

		if (ppCharsets.size() > 0 || ob.m_bDone)
		{
			String charsets = ob.m_bDone ? ob.m_Charset : ppCharsets[0];
			if (charsets.compare(wxT("windows-1252")) == 0)
			{
				for(size_t i = 0;i<ppCharsets.size();i++)
				{
					if (ppCharsets[i].compare(wxT("windows-1252")) != 0)
					{
						charsets = ppCharsets[i];
						break;
					}
				}
			}

			if (charsets.compare(wxT("Big5")) == 0)
			{
				return IDM_ENCODE_BIG5;
			}
			else if (charsets.compare(wxT("GB18030")) == 0)
			{
				return IDM_ENCODE_GB;
			}
			else if (charsets.compare(wxT("GB2312")) == 0)
			{
				return IDM_ENCODE_GB;
			}
			else if (charsets.compare(wxT("HZ-GB-2312")) == 0)
			{
				return IDM_ENCODE_GB;
			}
			else if (charsets.compare(wxT("UTF-16BE")) == 0)
			{
				return IDM_ENCODE_UNICODE_BE;
			}
			else if (charsets.compare(wxT("UTF-16LE")) == 0)
			{
				return IDM_ENCODE_UNICODE;
			}
			else if (charsets.compare(wxT("UTF-8")) == 0)
			{
				return IDM_ENCODE_UTF8;
			}
			else
			{
				return IDM_ENCODE_UNKNOWN;
			}
		}
		else
		{
			return IDM_ENCODE_UNKNOWN;
		}
	}
	else
	{
		det.DataEnd();

		return IDM_ENCODE_GB;
	}
}

bool CReadBookDoc::LoadBuffer(const wxString & url, wxMBConv * conv, bool bGuess)
{
	wxFileSystem fs;

	fs.ChangePathTo(url, false);

	wxFSFile * pFile = fs.OpenFile(url, wxFS_READ | wxFS_SEEKABLE);

	if (pFile == NULL)
		return false;

	wxInputStream * pInput = pFile->GetStream();

#if wxUSE_UNICODE
	wxMBConv * pConv = GetSuitableMBConv(pInput, conv, bGuess);

	wxTextInputStream text( *pInput, wxT("\t"), *pConv);
#else
	wxTextInputStream text( *pInput );
#endif

	while (!pInput->Eof())
	{
		wxString line = text.ReadLine();

		if (!IsEmptyLine(line))
			m_Buffer.Add(line);
	}

	delete pFile;

	return true;
}

wxMBConv * CReadBookDoc::GetSuitableMBConv(wxInputStream * pInput, wxMBConv * pDefaultConv, bool bGuess)
{
	wxMBConv * pConv = pDefaultConv;
	
	if (bGuess)
	{
		wxFileOffset offset = pInput->TellI();

		wxUint32 encoding = GuessDataEncoding(pInput);

		if (encoding != IDM_ENCODE_UNKNOWN)
		{
			CReadBookMainFrm * pMainFrame = (CReadBookMainFrm *)(GetMainFrame());
			pMainFrame->UpdateEncoding(encoding);
			pConv = pMainFrame->GetMBConv();
		}

		pInput->SeekI(offset);
	}

	return pConv;
}

wxFileOffset CReadBookDoc::GetRowOffset(wxInt32 nRow)
{
	return -1;
}

void CReadBookDoc::UpdateDisplay(wxReadBook::DisplayAsEnum displayAs,
								 wxReadBook::ViewModeEnum viewMode)
{
	::wxGetApp().GetPreference()->SetFileInfo(m_strFileName, 
		m_nCurrentLine, 
		GetRowOffset(m_nCurrentLine),
		displayAs,
		viewMode);
}