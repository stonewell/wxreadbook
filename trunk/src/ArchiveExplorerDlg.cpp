#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filesys.h"

#include <wx/listimpl.cpp>

#include "ReadBookApp.h"
#include "ArchiveExplorerDlg.h"

#ifndef __WXMSW__
#include "res/file.xpm"
#include "res/file32.xpm"
#include "res/folder.xpm"
#include "res/folder32.xpm"
#endif

#define ID_LIST_CTRL 1000

CArchiveItemData::CArchiveItemData(CArchiveItemData * pParentItem,
        const wxString & strUrl,
        bool bIsDir) :
    m_pParentItem(pParentItem),
    m_strUrl(strUrl),
    m_bIsDir(bIsDir)
{
}

CArchiveItemData::~CArchiveItemData()
{
}

WX_DEFINE_LIST(CArchiveItemDataPtrList);

/***************************ArchiveExplorerDlg Implmentation*********************************/
BEGIN_EVENT_TABLE(CArchiveExplorerDlg, wxDialog)
	EVT_LIST_ITEM_ACTIVATED(ID_LIST_CTRL, CArchiveExplorerDlg::OnItemActivate)
	EVT_SIZE(CArchiveExplorerDlg::OnSize)
END_EVENT_TABLE()

CArchiveExplorerDlg::CArchiveExplorerDlg(wxWindow * pParent, const wxString & archiveUrl) :
	wxDialog(pParent, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
	m_strArchiveUrl(archiveUrl),
	m_bmpFile(wxBITMAP(file)),
	m_bmpFile32(wxBITMAP(file32)),
	m_bmpFolder(wxBITMAP(folder)),
	m_bmpFolder32(wxBITMAP(folder32)),
	m_ImageList16(16,16),
	m_ImageList32(32,32),
	m_strSelectedFilePath(wxT(""))
{
	SetTitle(wxT("Select File From Archive"));

	m_ImageList16.Add(m_bmpFolder);
	m_ImageList16.Add(m_bmpFile);
	m_ImageList32.Add(m_bmpFolder32);
	m_ImageList32.Add(m_bmpFile32);

	m_pList = new wxListCtrl(this, 
		ID_LIST_CTRL, 
		wxDefaultPosition, wxSize(50,60), 
		wxLC_LIST | 
		wxSUNKEN_BORDER | wxLC_SINGLE_SEL |
		wxLC_NO_HEADER |
		wxHSCROLL |
		wxVSCROLL |
		wxLC_HRULES);
	m_pList->SetImageList(&m_ImageList16, wxIMAGE_LIST_SMALL);
	m_pList->SetImageList(&m_ImageList32, wxIMAGE_LIST_NORMAL);

	InitializeContent(CreateItemData(NULL, m_strArchiveUrl, true));

	wxButton *ok = new wxButton(this, wxID_OK, _T("OK"));
    ok->SetDefault();
	wxButton *cancel = new wxButton(this, wxID_CANCEL, _T("Cancel"));

	wxSize clientSize = pParent->GetClientSize();

	int width = clientSize.GetWidth() * 2 / 3;
	int height = clientSize.GetHeight();

	wxSize size = ok->GetSize();

	wxInt32 btnWidth = size.GetWidth() * 2 + 5 * 3;

#ifdef __WXGPE__
printf("%d,%d,%d\n", width, btnWidth, size.GetWidth());
#endif

	if (width != 240)
		width = 240;

	if (width < btnWidth)
		width = btnWidth;

	SetSize(width, width / 5 * 6);
	SetPosition(wxPoint(clientSize.GetWidth()/ 2 - width / 2,
		clientSize.GetHeight() / 2 - width / 5 * 6 / 2));

	clientSize = GetClientSize();

	m_pList->SetSize(5,5,clientSize.GetWidth() - 10, clientSize.GetHeight() - 5 - 10 - size.GetHeight());

	ok->SetPosition(wxPoint(clientSize.GetWidth() - 5 - size.GetWidth(),
		clientSize.GetHeight() - 5 - size.GetHeight())); 
	cancel->SetPosition(wxPoint(clientSize.GetWidth() - 5 - size.GetWidth() - 
			5 - size.GetWidth(),
		clientSize.GetHeight() - 5 - size.GetHeight()));
}

CArchiveExplorerDlg::~CArchiveExplorerDlg()
{
    ClearListItem();
}

void CArchiveExplorerDlg::InitializeContent(CArchiveItemData * pParentItem)
{
	wxFileSystem fs;

    ChangePathToItem(fs, pParentItem);

   	m_pList->ClearAll();

	int flag = wxDIR;

	long index = -1;

    if (pParentItem != NULL && pParentItem->GetUrl().CmpNoCase(m_strArchiveUrl) != 0)
    {
        CArchiveItemData * pItem = CreateItemData(pParentItem, wxT(".."), true);
        index = m_pList->InsertItem(index + 1, wxT(".."), 0);
        m_pList->SetItemData(index, reinterpret_cast<long>(pItem));
    }

	for(int i=0;i<2;i++)
	{
		wxString name = fs.FindFirst(wxT("*"), flag);

		while (name.Length() > 0)
		{
			wxFileName fn(name);

			if (IsFileNameMatch(fn) || flag == wxDIR)
			{
				if (flag == wxDIR)
				{
				    CArchiveItemData * pItem = CreateItemData(pParentItem, name, true);
					index = m_pList->InsertItem(index + 1, fn.GetFullName(), i);
					m_pList->SetItemData(index, reinterpret_cast<long>(pItem));
				}
				else
				{
				    CArchiveItemData * pItem = CreateItemData(pParentItem, name, false);

                    name = fn.GetFullName();

					int tmpIndex = name.Last(':');

					if (tmpIndex > 0)
					{
						name = name.SubString(tmpIndex + 1, name.Length());
					}

					index = m_pList->InsertItem(index + 1, name, i);

					m_pList->SetItemData(index, reinterpret_cast<long>(pItem));
				}
			}

			name = fs.FindNext();
		}

		flag = wxFILE;
	}
}

void CArchiveExplorerDlg::OnItemActivate(wxListEvent & WXUNUSED(event))
{
    long item = m_pList->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    if ( item == -1 )
        return;

    CArchiveItemData * pData =
        reinterpret_cast<CArchiveItemData *>(m_pList->GetItemData(item));

    if (pData->IsDir())
    {
        CArchiveItemData * pParentItem = pData;

        if (pData->GetUrl().CmpNoCase(wxT("..")) == 0)
        {
            pParentItem = pParentItem->GetParentItem()->GetParentItem();
        }

        InitializeContent(pParentItem);
    }
    else
    {
        m_strSelectedFilePath = pData->GetUrl();

        EndModal(wxID_OK);
    }
}

void CArchiveExplorerDlg::ClearListItem()
{
    CArchiveItemDataPtrList::iterator it;

    for(it = m_ItemDataList.begin(); it != m_ItemDataList.end(); it++)
    {
        CArchiveItemData * pData = *it;

        if (pData != NULL)
            delete pData;
    }
}

CArchiveItemData * CArchiveExplorerDlg::CreateItemData(CArchiveItemData * pParentItem,
    const wxString & url, bool bIsDir)
{
    CArchiveItemData * pItem = new CArchiveItemData(pParentItem, url, bIsDir);

    m_ItemDataList.Append(pItem);

    return pItem;
}

void CArchiveExplorerDlg::ChangePathToItem(wxFileSystem & fs, CArchiveItemData * pParentItem)
{
    if (pParentItem != NULL)
    {
        ChangePathToItem(fs, pParentItem->GetParentItem());

        fs.ChangePathTo(pParentItem->GetUrl(),true);
    }
}

const wxString & CArchiveExplorerDlg::GetSelectedFilePath()
{
    if (m_strSelectedFilePath.Length() > 0)
        return m_strSelectedFilePath;

    long item = m_pList->GetNextItem(-1,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
    if ( item == -1 )
        return m_strSelectedFilePath;

    CArchiveItemData * pData =
        reinterpret_cast<CArchiveItemData *>(m_pList->GetItemData(item));

    if (pData->IsDir())
    {
        return m_strSelectedFilePath;
    }
    else
    {
        return pData->GetUrl();
    }
}

void CArchiveExplorerDlg::OnSize(wxSizeEvent& event)
{
}

