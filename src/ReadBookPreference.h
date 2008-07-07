#ifndef __READBOOK_PREFERENCE_H__
#define __READBOOK_PREFERENCE_H__

#include <wx/datstrm.h>

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

	void SetFileInfo(const wxString & strFileName, wxInt32 nCurrentLine, wxInt32 nFilePos);
	wxInt32 GetFileInfo(const wxString & strFileName, CFileInfo ** ppFileInfo = NULL);

	const wxString & GetLastFile() const { return m_strLastFile; }

	wxColour GetTextColor() const { return m_cTxtColor; }
	void SetTextColor(wxColour color) { m_cTxtColor = color; }

	wxColour GetBackgroundColor() const { return m_cBkColor; }
	void SetBackgroundColor(wxColour color) { m_cBkColor = color; }

	const CFileInfoList * GetFileInfoMap() { return &m_FileInfoList; }
};
#endif

