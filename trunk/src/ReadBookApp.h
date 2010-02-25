#ifndef READBOOKAPP_H_
#define READBOOKAPP_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

#include "ReadBookHelperFunc.h"
#include "ReadBookIds.h"

#endif /*READBOOKAPP_H_*/
