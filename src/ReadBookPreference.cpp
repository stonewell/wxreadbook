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

#ifdef __WXMAC__
#include "wx/filename.h"
#endif

#include <wx/font.h>
#include "wx/filesys.h"

#include "ReadBookApp.h"
#include "ReadBookPreference.h"
#include <wx/listimpl.cpp>

WX_DEFINE_LIST(CFileInfoList);

////////////////////////////////////////////////////////////
////
////CFileInfo
////
////////////////////////////////////////////////////////////
CFileInfo::CFileInfo(void) :
m_strFileName(wxT("")),
m_nCurrentLine(0),
m_nFilePos(0),
m_nDisplayAs(wxReadBook::DisplayAsOriginal),
m_nViewMode(wxReadBook::Text)
{
}

CFileInfo::~CFileInfo(void)
{
}

void CFileInfo::Store(wxDataOutputStream & archive)
{
	bool isDir = false;

	archive << FileNameToUrl(m_strFileName, isDir);

	archive << m_nCurrentLine;
	archive << m_nFilePos;

	wxInt32 tmpVal = m_nDisplayAs;

	archive << tmpVal;

	tmpVal = m_nViewMode;

	archive << tmpVal;
}

void CFileInfo::Load(wxInt32 version, wxDataInputStream & archive)
{
	archive >> m_strFileName;

	bool isDir = false;
	m_strFileName = FileNameToUrl(m_strFileName, isDir);

	archive >> m_nCurrentLine;
	archive >> m_nFilePos;

	if (version >= 2)
	{
		wxInt32 tmpVal = 0;

		archive >> tmpVal;
		m_nDisplayAs = (wxReadBook::DisplayAsEnum)tmpVal;

		archive >> tmpVal;
		m_nViewMode = (wxReadBook::ViewModeEnum)tmpVal;
	}
}

////////////////////////////////////////////////////////////
////
////CReadBookPreference
////
////////////////////////////////////////////////////////////
const wxInt32 PREFERENCE_VERSION = 2;

CReadBookPreference::CReadBookPreference(void):
m_pLogFont(NULL)
{
	Initialize();
}

CReadBookPreference::~CReadBookPreference(void)
{
	ClearFileInfoMap();

	if (m_pLogFont != NULL)
		delete m_pLogFont;
}

void CReadBookPreference::Initialize()
{
	ClearFileInfoMap();

	m_pLogFont = wxFont::New(
		48,
		wxFONTFAMILY_DEFAULT,
		wxFONTFLAG_BOLD | wxFONTFLAG_ANTIALIASED,
		wxEmptyString);

	m_nLineMargin = 20;
	m_nColumnMargin = 5;

	m_strLastFile = wxEmptyString;

#ifdef __WXMSW__
	m_cTxtColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
	m_cBkColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
#else
	m_cTxtColor = *wxBLACK;
	m_cBkColor = *wxWHITE;
#endif
}

void CReadBookPreference::ClearFileInfoMap()
{
	CFileInfoMap::iterator it;

	for(it = m_FileInfoMap.begin();it != m_FileInfoMap.end(); ++it)
	{
		wxString key = it->first;
		CFileInfo * pFileInfo = it->second;

		delete pFileInfo;
	}

	m_FileInfoMap.clear();
	m_FileInfoList.clear();
}

void CReadBookPreference::SetFont(wxFont * lplf)
{
	if (m_pLogFont != NULL)
		delete m_pLogFont;

	m_pLogFont = new wxFont(*lplf);
}

wxFont * CReadBookPreference::GetFont() const
{
	return m_pLogFont;
}

wxInt16 CReadBookPreference::GetLineMargin() const
{
	return m_nLineMargin;
}

void CReadBookPreference::SetLineMargin(wxInt16 nLineMargin)
{
	m_nLineMargin = nLineMargin;
}

wxInt16 CReadBookPreference::GetColumnMargin() const
{
	return m_nColumnMargin;
}

void CReadBookPreference::SetColumnMargin(wxInt16 nColumnMargin)
{
	m_nColumnMargin = nColumnMargin;
}

void CReadBookPreference::Load( wxInputStream & input )
{
	wxString strFaceName;
	wxInt16 nFamily;
	wxInt16 nPointSize;
	wxInt16 nStyle;
	wxInt16 nEncoding;
	wxInt16 noAnti;
	wxInt16 Underlined;
	wxInt16 nWeight;
	wxString strTxtColor;
	wxString strBkColor;
	wxString strLastFile;

	wxDataInputStream archive(input);

	wxInt32 version = 0;

	archive >> version;

	if (version < 1 || version > PREFERENCE_VERSION)
	{
		input.SeekI(0);

		version = 1;
	}

	archive >> nFamily >> strFaceName
		>> nPointSize >> nStyle
		>> nEncoding >> noAnti
		>> Underlined >> nWeight
		>> m_nLineMargin >> m_nColumnMargin
		>> strLastFile
		>> strTxtColor
		>> strBkColor;

	m_cTxtColor.Set(strTxtColor);
	m_cBkColor.Set(strBkColor);

	m_pLogFont = wxFont::New(nPointSize,nFamily,nStyle | wxFONTFLAG_ANTIALIASED,nWeight,Underlined == 1,
		strFaceName,static_cast<wxFontEncoding>(nEncoding));

	m_pLogFont->SetNoAntiAliasing(noAnti == 1);

	wxInt32 count = 0;

	archive >> count;

	wxFileSystem fs;

	for(wxInt32 i = 0;i<count;i++)
	{
		wxString key;

		archive >> key;

		CFileInfo * pFileInfo = new CFileInfo();
		pFileInfo->Load(version, archive);

		wxFSFile * pFile = fs.OpenFile(key, wxFS_READ);

		if (!key.IsEmpty() && pFile != NULL)
		{
			SetFileInfo(pFileInfo->m_strFileName,
				pFileInfo->m_nCurrentLine,
				pFileInfo->m_nFilePos,
				pFileInfo->m_nDisplayAs,
				pFileInfo->m_nViewMode);
		}

		if (pFile != NULL)
		{
			delete pFile;
		}

		delete pFileInfo;
	}

	m_strLastFile = strLastFile;
}

void CReadBookPreference::Store( wxOutputStream & output )
{
	wxString strFaceName = m_pLogFont->GetFaceName();
	wxInt16 nFamily = m_pLogFont->GetFamily();
	wxInt16 nPointSize = m_pLogFont->GetPointSize();
	wxInt16 nStyle = m_pLogFont->GetStyle();
	wxInt16 nEncoding = static_cast<wxInt16>(m_pLogFont->GetEncoding());
	wxInt16 noAnti = (m_pLogFont->GetNoAntiAliasing() ? (wxInt16)1 : (wxInt16)0);
	wxInt16 Underlined = (m_pLogFont->GetUnderlined() ? (wxInt16)1 : (wxInt16)0);
	wxInt16 nWeight = static_cast<wxInt16>(m_pLogFont->GetWeight());

	wxDataOutputStream archive(output);

	archive << PREFERENCE_VERSION;

	archive << nFamily << strFaceName
		<< nPointSize << nStyle
		<< nEncoding << noAnti
		<< Underlined << nWeight
		<< m_nLineMargin << m_nColumnMargin
		<< m_strLastFile
		<< m_cTxtColor.GetAsString(wxC2S_HTML_SYNTAX)
		<< m_cBkColor.GetAsString(wxC2S_HTML_SYNTAX);

	wxInt32 count = m_FileInfoMap.size();
	archive << count;

	CFileInfoList::reverse_iterator it;

	for(it = m_FileInfoList.rbegin();it != m_FileInfoList.rend(); ++it)
	{
		CFileInfo * pFileInfo = *it;

		archive << pFileInfo->m_strFileName;

		pFileInfo->Store(archive);
	}
}

void CReadBookPreference::SetFileInfo(const wxString & strFileName,
									  wxInt32 nCurrentLine,
									  wxInt32 nFilePos,
									  wxReadBook::DisplayAsEnum displayAs,
									  wxReadBook::ViewModeEnum viewMode)
{
	CFileInfo * pFileInfo = NULL;

	bool bCreateNew = false;

	CFileInfoMap::iterator it = m_FileInfoMap.find(strFileName);

	if (it == m_FileInfoMap.end())
	{
		pFileInfo = new CFileInfo();
		bCreateNew = true;
	}
	else
	{
		pFileInfo = it->second;
	}

	pFileInfo->m_strFileName = strFileName;
	pFileInfo->m_nCurrentLine = nCurrentLine;
	pFileInfo->m_nFilePos = nFilePos;

	if (displayAs != wxReadBook::DisplayAsUnknown)
		pFileInfo->m_nDisplayAs = displayAs;

	if (viewMode != wxReadBook::ViewModeUnknown)
		pFileInfo->m_nViewMode = viewMode;

	if (bCreateNew)
	{
		m_FileInfoMap[strFileName] = pFileInfo;
	}

	m_strLastFile = strFileName;

	if (!bCreateNew)
	{
		m_FileInfoList.remove(pFileInfo);
	}

	m_FileInfoList.Insert((size_t)0, pFileInfo);
}

wxInt32 CReadBookPreference::GetFileInfo(const wxString & strFileName,
										 CFileInfo ** ppFileInfo)
{
	CFileInfoMap::iterator it = m_FileInfoMap.find(strFileName);

	if (it != m_FileInfoMap.end())
	{
		if (ppFileInfo != NULL)
		{
			*ppFileInfo = it->second;
		}

		return it->second->m_nCurrentLine;
	}

	return 0;
}
