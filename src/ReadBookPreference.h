#ifndef __READBOOK_PREFERENCE_H__
#define __READBOOK_PREFERENCE_H__

#include <wx/datstrm.h>

#include "lib7zip.h"

namespace wxReadBook
{
	enum DisplayAsEnum
	{
		DisplayAsUnknown,
		DisplayAsOriginal,
		DisplayAsSimplify,
		DisplayAsTraditional
	};

	enum ViewModeEnum
	{
		ViewModeUnknown,
		Html,
		Text
	};
}

class CFileInfo
{
public:
	CFileInfo();
	virtual ~CFileInfo();

	void Store(wxDataOutputStream & archive);
	void Load(wxInt32 version, wxDataInputStream & archive);

public:
	wxString m_strFileName;
	wxInt32 m_nCurrentLine;
	wxInt32 m_nFilePos;
	wxReadBook::DisplayAsEnum m_nDisplayAs;
	wxReadBook::ViewModeEnum m_nViewMode;
};

WX_DECLARE_LIST(CFileInfo, CFileInfoList);

WX_DECLARE_HASH_MAP( wxString, CFileInfo*, wxStringHash, wxStringEqual, CFileInfoMap);

class CReadBookPreference
{
public:
	CReadBookPreference(void);
	virtual ~CReadBookPreference(void);

	void Store(wxOutputStream & input);
	void Load(wxInputStream & output);

private:
	wxFont * m_pLogFont;
	wxInt16 m_nLineMargin;
	wxInt16 m_nColumnMargin;
	wxColour m_cTxtColor;
	wxColour m_cBkColor;

	CFileInfoMap m_FileInfoMap;
	CFileInfoList m_FileInfoList;

	wxString m_strLastFile;

	C7ZipLibrary m_7ZipLibrary;
	WStringArray m_Exts;

	void Initialize();

	void ClearFileInfoMap();

protected:

public:
	void SetFont(wxFont * lplf);
	wxFont * GetFont() const;

	wxInt16 GetLineMargin() const;
	void SetLineMargin(wxInt16 nLineMargin);

	wxInt16 GetColumnMargin() const;
	void SetColumnMargin(wxInt16 nColMargin);

	void SetFileInfo(const wxString & strFileName, 
		wxInt32 nCurrentLine, 
		wxInt32 nFilePos,
		wxReadBook::DisplayAsEnum displayAs = wxReadBook::DisplayAsUnknown,
		wxReadBook::ViewModeEnum viewMode = wxReadBook::ViewModeUnknown);
	wxInt32 GetFileInfo(const wxString & strFileName, CFileInfo ** ppFileInfo = NULL);

	const wxString & GetLastFile() const { return m_strLastFile; }

	wxColour GetTextColor() const { return m_cTxtColor; }
	void SetTextColor(wxColour color) { m_cTxtColor = color; }

	wxColour GetBackgroundColor() const { return m_cBkColor; }
	void SetBackgroundColor(wxColour color) { m_cBkColor = color; }

	const CFileInfoList * GetFileInfoMap() { return &m_FileInfoList; }

	bool IsArchiveExt(const wxString & ext) const;

	wxString GetArchiveFileFilters();
};
#endif

