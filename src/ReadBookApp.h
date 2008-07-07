#ifndef READBOOKAPP_H_
#define READBOOKAPP_H_

#include "wx/docview.h"
#include "wx/filename.h"

class wxDocManager;

#include "ReadBookPreference.h"

// Define a new application
class CReadBookApp: public wxApp
{
public:
    CReadBookApp(void);
    bool OnInit(void);
    int OnExit(void);

	CReadBookPreference * GetPreference() { return &m_ReadBookPreference; }

    wxDocManager * GetDocManager() { return m_pDocManager; }

    void OpenRelatedFile(const wxString & fileName, wxInt16 nDelta);

protected:
    wxDocManager* m_pDocManager;

private:
	CReadBookPreference m_ReadBookPreference;

	void LoadPreference();
	void SavePreference();

	const wxString GetConfigFileName();
};

DECLARE_APP(CReadBookApp)

extern wxFrame *GetMainFrame(void);

extern bool IsFileNameMatch(const wxFileName & filename);

extern const wxString FileNameToUrl(const wxString & filename, bool & isUrlDir);
#endif /*READBOOKAPP_H_*/
