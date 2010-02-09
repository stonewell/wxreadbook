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

#include "wx/generic/filedlgg.h"
#include "wx/docview.h"

#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/stdpaths.h"
#include "wx/filesys.h"
#include "wx/fs_arc.h"
#include "wx/fs_filter.h"

#include "ReadBookApp.h"
#include "ReadBookMainFrm.h"
#include "ReadBookCanvas.h"
#include "ArchiveExplorerDlg.h"

extern CReadBookMainFrm * g_pMainFrm;

CReadBookMainFrm *GetMainFrame(void)
{
    return g_pMainFrm;
}

wxMBConv * GetCurrentMBConv(void)
{
	return g_pMainFrm->GetMBConv();
}

bool IsArchiveExt(const wxString & ext)
{
	return ext.CmpNoCase(wxT("zip")) == 0 ||
		ext .CmpNoCase(wxT("gz")) == 0 ||
		ext.CmpNoCase(wxT("tar")) == 0 ||
		wxGetApp().GetPreference()->IsArchiveExt(ext);
}

bool IsFileNameMatch(const wxFileName & filename)
{
	return 	filename.GetExt().CmpNoCase(wxT("txt")) == 0 ||
			filename.GetExt().CmpNoCase(wxT("html")) == 0  ||
			filename.GetExt().CmpNoCase(wxT("htm")) == 0  ||
			IsArchiveExt(filename.GetExt());
}

const wxString FileNameToUrl(const wxString & filename, bool & isUrlDir)
{
	wxString tmpfilename = filename;
	
	tmpfilename.Replace(wxT("\\"), wxT("/"));

//	wxFileName name(tmpfilename);

	wxString url = tmpfilename;

	size_t pos = tmpfilename.rfind(wxT("."));
	size_t begin = tmpfilename.Len() - 1;

	while(pos != wxString::npos)
	{
		wxString ext = tmpfilename.SubString(pos + 1, begin);

		if (IsArchiveExt(ext))
		{
			if (ext.CmpNoCase(wxT("gz")) == 0)
			{
				url += wxT("#gzip:");
			}
			else
			{
				url += wxT("#");
				url += ext;
				url += wxT(":");
			}
		}
		else
		{
			break;
		}

		begin = pos - 1;

		pos = tmpfilename.rfind(wxT("."), begin);
	}

	isUrlDir = url.CmpNoCase(tmpfilename) != 0;

	if (isUrlDir)
	{
		url += wxT("/");
	}

	//if (filename.Lower().EndsWith(wxT(".tar.gz")))
	//{
	//	url = tmpfilename + wxT("#gzip:#tar:/");
	//}
	//else if (filename.Lower().EndsWith(wxT(".rar.gz")))
	//{
	//	url = tmpfilename + wxT("#gzip:#rar:/");
	//}
	//else if (name.GetExt().CmpNoCase(wxT("zip")) == 0)
	//{
	//	url = tmpfilename + wxT("#zip:/");
	//}
	//else if (name.GetExt().CmpNoCase(wxT("gz")) == 0)
	//{
	//	url = tmpfilename + wxT("#gzip:/");
	//}
	//else if (name.GetExt().CmpNoCase(wxT("tar")) == 0)
	//{
	//	url = tmpfilename + wxT("#tar:/");
	//}
	//else if (name.GetExt().CmpNoCase(wxT("rar")) == 0)
	//{
	//	url = tmpfilename + wxT("#rar:/");
	//}
	//else
	//{
	//	url = tmpfilename;//name.GetFullPath();

	//	isUrlDir = false;
	//}

	return url;
}

bool ChooseArchiveFile(const wxString & archiveUrl, wxString & choosedArchiveFile)
{
	CArchiveExplorerDlg arDlg(GetMainFrame(),archiveUrl);

	if (arDlg.ShowModal() == wxID_OK)
	{
		choosedArchiveFile = arDlg.GetSelectedFilePath();

		return true;
	}
	else
	{
		return false;
	}
}

CReadBookView * GetCurrentView()
{
	return (CReadBookView *)GetCurrentCanvas()->GetView();
}

void SetMainFrameTitle(const wxString & strTitle)
{
	GetMainFrame()->SetTitle(strTitle);
}

void AddToRecentFile(const wxString & strFileName)
{
	GetMainFrame()->AddRecentFile(strFileName);
}

void UpdateCurrentEncoding(wxUint32 encoding)
{
	GetMainFrame()->UpdateEncoding(encoding);
}

CReadBookCanvas * GetCurrentCanvas()
{
	return GetMainFrame()->GetCanvas();
}
