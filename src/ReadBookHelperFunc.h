#pragma once

class CReadBookMainFrm;
class CReadBookView;
class CReadBookCanvas;

CReadBookMainFrm *GetMainFrame(void);

bool IsFileNameMatch(const wxFileName & filename);

const wxString FileNameToUrl(const wxString & filename, bool & isUrlDir);

wxMBConv * GetCurrentMBConv(void);

bool ChooseArchiveFile(const wxString & archiveUrl, wxString & choosedArchiveFile);

void SetMainFrameTitle(const wxString & strTitle);
void AddToRecentFile(const wxString & strFileName);
void UpdateCurrentEncoding(wxUint32 encoding);

CReadBookView * GetCurrentView();
CReadBookCanvas * GetCurrentCanvas();
