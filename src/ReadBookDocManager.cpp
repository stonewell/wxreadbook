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
#include "ReadBookDocManager.h"

static wxWindow* wxFindSuitableParent();

wxDocTemplate *CReadBookDocManager::SelectDocumentPath(wxDocTemplate **templates,
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
                                                int noTemplates,
#else
                                                int WXUNUSED(noTemplates),
#endif
                                                wxString& path,
                                                long WXUNUSED(flags),
                                                bool WXUNUSED(save))
{
    // We can only have multiple filters in Windows and GTK
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
    wxString descrBuf;

    int i;
    for (i = 0; i < noTemplates; i++)
    {
        if (templates[i]->IsVisible())
        {
            // add a '|' to separate this filter from the previous one
            if ( !descrBuf.empty() )
                descrBuf << wxT('|');

            descrBuf << templates[i]->GetDescription()
                << wxT(" |")
                //<< wxT(" (") << templates[i]->GetFileFilter() << wxT(") |")
                << templates[i]->GetFileFilter();
        }
    }
#else
    wxString descrBuf = wxT("*.*");
#endif

    int FilterIndex = -1;

    wxWindow* parent = wxFindSuitableParent();

#ifdef _WIN32_WCE
    wxGenericFileDialog fileDialog(parent,
                            _("Select a file"),
                            m_lastDirectory,
                            wxEmptyString,
                            descrBuf);

    wxString pathTmp;
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        FilterIndex = fileDialog.GetFilterIndex();

        pathTmp = fileDialog.GetPath();
    }
#else
    wxString pathTmp = wxFileSelectorEx(_("Select a file"),
                                        m_lastDirectory,
                                        wxEmptyString,
                                        &FilterIndex,
                                        descrBuf,
                                        0,
                                        parent);
#endif
    wxDocTemplate *theTemplate = (wxDocTemplate *)NULL;
    if (!pathTmp.empty())
    {
        if (!wxFileExists(pathTmp))
        {
            wxString msgTitle;
            if (!wxTheApp->GetAppName().empty())
                msgTitle = wxTheApp->GetAppName();
            else
                msgTitle = wxString(_("File error"));

            (void)wxMessageBox(_("Sorry, could not open this file."), msgTitle, wxOK | wxICON_EXCLAMATION,
                parent);

            path = wxEmptyString;
            return (wxDocTemplate *) NULL;
        }
        m_lastDirectory = wxPathOnly(pathTmp);

        path = pathTmp;

        // first choose the template using the extension, if this fails (i.e.
        // wxFileSelectorEx() didn't fill it), then use the path
        if ( FilterIndex != -1 )
            theTemplate = templates[FilterIndex];
        if ( !theTemplate )
            theTemplate = FindTemplateForPath(path);
        if ( !theTemplate )
        {
            // Since we do not add files with non-default extensions to the FileHistory this
            // can only happen if the application changes the allowed templates in runtime.
            (void)wxMessageBox(_("Sorry, the format for this file is unknown."),
                                _("Open File"),
                                wxOK | wxICON_EXCLAMATION, wxFindSuitableParent());
        }
    }
    else
    {
        path = wxEmptyString;
    }

    return theTemplate;
}

static wxWindow* wxFindSuitableParent()
{
    wxWindow* parent = wxTheApp->GetTopWindow();

    wxWindow* focusWindow = wxWindow::FindFocus();
    if (focusWindow)
    {
        while (focusWindow &&
                !focusWindow->IsKindOf(CLASSINFO(wxDialog)) &&
                !focusWindow->IsKindOf(CLASSINFO(wxFrame)))

            focusWindow = focusWindow->GetParent();

        if (focusWindow)
            parent = focusWindow;
    }
    return parent;
}
