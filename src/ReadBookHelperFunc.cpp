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

bool CanHandleFile(const wxFileName & filename)
{
	return 	filename.GetExt().CmpNoCase(wxT("txt")) == 0 ||
		filename.GetExt().CmpNoCase(wxT("html")) == 0  ||
		filename.GetExt().CmpNoCase(wxT("htm")) == 0  ||
		IsArchiveExt(filename.GetExt());
}

bool IsArchiveFileUrl(const wxString & url)
{
	return url.find(wxT('#')) != wxString::npos;
}

const wxString FileNameToUrl(const wxString & filename, bool & isUrlDir)
{
	wxString tmpfilename = filename;

	tmpfilename.Replace(wxT("\\"), wxT("/"));

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

void UpdateCurrentEncoding(const wxString & charset)
{
	GetMainFrame()->UpdateEncoding(charset);
}

CReadBookCanvas * GetCurrentCanvas()
{
	return GetMainFrame()->GetCanvas();
}

wxInt32 CharsetToMenuID(const wxString & charset)
{
	if (charset.compare(wxT("windows-1252")) == 0)
	{
		return IDM_ENCODE_WINDOWS_1252;
	}
	else if (charset.compare(wxT("Big5")) == 0)
	{
		return IDM_ENCODE_BIG5;
	}
	else if (charset.compare(wxT("GB18030")) == 0)
	{
		return IDM_ENCODE_GB;
	}
	else if (charset.compare(wxT("GB2312")) == 0)
	{
		return IDM_ENCODE_GB;
	}
	else if (charset.compare(wxT("HZ-GB-2312")) == 0)
	{
		return IDM_ENCODE_GB;
	}
	else if (charset.compare(wxT("UTF-16BE")) == 0)
	{
		return IDM_ENCODE_UNICODE_BE;
	}
	else if (charset.compare(wxT("UTF-16LE")) == 0)
	{
		return IDM_ENCODE_UNICODE;
	}
	else if (charset.compare(wxT("UTF-7")) == 0)
	{
		return IDM_ENCODE_UTF7;
	}
	else if (charset.compare(wxT("UTF-8")) == 0)
	{
		return IDM_ENCODE_UTF8;
	}
	else
	{
		return IDM_ENCODE_CHARSET;
	}
}

#if !defined(_WIN32) || !defined(WINDOWSCE)
extern wxMBConv* new_wxMBConv_iconv( const wxChar* name );
#endif

wxMBConv * CreateEncoding(const wxString & charset)
{
	wxInt32 id = CharsetToMenuID(charset);
	wxMBConv * pMBConv = NULL;

	switch(id)
	{
	case IDM_ENCODE_WINDOWS_1252:
		{
#if defined(_WIN32) || defined(WINDOWSCE)
			pMBConv = new wxCSConv(wxFONTENCODING_CP1252);
#else
			pMBConv = new_wxMBConv_iconv(wxT("WINDOWS-1252"));
#endif
			break;
		}
	case IDM_ENCODE_GB:
		{
#if defined(_WIN32) || defined(WINDOWSCE)
			pMBConv = new wxCSConv(wxFONTENCODING_CP936);
#else
			pMBConv = new_wxMBConv_iconv(wxT("GB18030"));
#endif
			break;
		}
	case IDM_ENCODE_BIG5:
		{
#if defined(_WIN32) || defined(WINDOWSCE)
			pMBConv = new wxCSConv(wxFONTENCODING_CP950);
#else
			pMBConv = new_wxMBConv_iconv(wxT("BIG5"));
#endif
			break;
		}
	case IDM_ENCODE_UTF7:
		{
			pMBConv = new wxMBConvUTF7;
			break;
		}
	case IDM_ENCODE_UTF8:
		{
			pMBConv = new wxMBConvUTF8;
			break;
		}
	case IDM_ENCODE_UNICODE:
		{
			pMBConv = new wxMBConvUTF16LE;
			break;
		}
	case IDM_ENCODE_UNICODE_BE:
		{
			pMBConv = new wxMBConvUTF16BE;
			break;
		}
	case IDM_ENCODE_UNICODE_32:
		{
			pMBConv = new wxMBConvUTF32LE;
			break;
		}
	case IDM_ENCODE_UNICODE_32_BE:
		{
			pMBConv = new wxMBConvUTF32BE;
			break;
		}
	case IDM_ENCODE_CHARSET:
		{
#if defined(_WIN32) || defined(WINDOWSCE)
			pMBConv = new wxCSConv(charset);
#else
			pMBConv = new_wxMBConv_iconv(charset);
#endif
		}
	default:
		break;
	}

	return pMBConv;
}

wxString MenuIDToCharset(wxInt32 id)
{
	switch(id)
	{
	case IDM_ENCODE_WINDOWS_1252:
		{
			return wxT("windows-1252");
		}
	case IDM_ENCODE_BIG5:
		{
			return wxT("Big5");
		}
	case IDM_ENCODE_UTF7:
		{
			return wxT("UTF-7");
		}
	case IDM_ENCODE_UTF8:
		{
			return wxT("UTF-8");
		}
	case IDM_ENCODE_UNICODE:
		{
			return wxT("UTF-16LE");
		}
	case IDM_ENCODE_UNICODE_BE:
		{
			return wxT("UTF-16BE");
		}
	case IDM_ENCODE_UNICODE_32:
		{
			return wxT("UTF-32LE");
		}
	case IDM_ENCODE_UNICODE_32_BE:
		{
			return wxT("UTF-32BE");
		}
	case IDM_ENCODE_CHARSET:
		{
			return GetCharsetMenuItemText();
		}
	case IDM_ENCODE_GB:
	default:
		break;
	}

	return wxT("GB18030");
}

void UpdateCharsetMenuItemText(const wxString & text)
{
	GetMainFrame()->UpdateCharsetMenuItemText(text);
}

wxString GetCharsetMenuItemText()
{
	return GetMainFrame()->GetCharsetMenuItemText();
}

wxInt32 GetCurrentLang()
{
	return GetMainFrame()->GetCurrentLang();
}