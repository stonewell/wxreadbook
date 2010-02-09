#pragma once

class CReadBookDocManager : public wxDocManager
{
public:
    CReadBookDocManager(long flags = wxDEFAULT_DOCMAN_FLAGS, bool initialize = true) :
      wxDocManager(flags, initialize)
    {
    }

    virtual ~CReadBookDocManager()
    {
    }

public:
    virtual wxDocTemplate *SelectDocumentPath(wxDocTemplate **templates,
            int noTemplates, wxString& path, long flags, bool save = false);
};
