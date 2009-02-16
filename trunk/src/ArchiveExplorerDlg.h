#ifndef ARCHIVEEXPLORERDLG_H_
#define ARCHIVEEXPLORERDLG_H_

#include "wx/dialog.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/bitmap.h"
#include "wx/imaglist.h"
#include "wx/listctrl.h"
#include "wx/list.h"

class CArchiveItemData
{
public:
    CArchiveItemData(CArchiveItemData * pParentItem,
        const wxString & strUrl,
        bool bIsDir);
    virtual ~CArchiveItemData();

private:
    CArchiveItemData * m_pParentItem;

    wxString m_strUrl;

    bool m_bIsDir;

public:
    CArchiveItemData * GetParentItem() { return m_pParentItem; }
    const wxString & GetUrl() { return m_strUrl; }

    bool IsDir() { return m_bIsDir; }
};

WX_DECLARE_LIST(CArchiveItemData, CArchiveItemDataPtrList);

class CArchiveExplorerDlg : public wxDialog
{
public:
	CArchiveExplorerDlg(wxWindow * pParent, const wxString & archiveUrl);
	virtual ~CArchiveExplorerDlg();

private:
	wxString m_strarchiveUrl;
	wxBitmap m_bmpFile;
	wxBitmap m_bmpFile32;
	wxBitmap m_bmpFolder;
	wxBitmap m_bmpFolder32;

	wxImageList m_ImageList16;
	wxImageList m_ImageList32;

    wxString m_strSelectedFilePath;

    CArchiveItemDataPtrList m_ItemDataList;

	wxListCtrl * m_pList;

	void InitializeContent(CArchiveItemData * pParentItem);

    void ClearListItem();

    CArchiveItemData * CreateItemData(CArchiveItemData * pParentItem, const wxString & url, bool bIsDir);

    void ChangePathToItem(wxFileSystem & fs,CArchiveItemData * pParentItem);

    DECLARE_EVENT_TABLE()

public:
	void OnItemActivate(wxListEvent & event);
	const wxString & GetSelectedFilePath();
    void OnSize(wxSizeEvent& event);
};

#endif /*ARCHIVEEXPLORERDLG_H_*/
