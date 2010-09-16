#pragma once

class CReadBookMainFrm;
class CReadBookView;
class CReadBookCanvas;

CReadBookMainFrm *GetMainFrame(void);

bool CanHandleFile(const wxFileName & filename);
bool IsArchiveFileUrl(const wxString & url);

const wxString FileNameToUrl(const wxString & filename, bool & isUrlDir);

wxMBConv * GetCurrentMBConv(void);

bool ChooseArchiveFile(const wxString & archiveUrl, wxString & choosedArchiveFile);

void SetMainFrameTitle(const wxString & strTitle);
void AddToRecentFile(const wxString & strFileName);

void UpdateCurrentEncoding(const wxString & charset);

CReadBookView * GetCurrentView();
CReadBookCanvas * GetCurrentCanvas();

wxInt32 CharsetToMenuID(const wxString & charsets);
wxString MenuIDToCharset(wxInt32 id);
wxMBConv * CreateEncoding(const wxString & charset);

void UpdateCharsetMenuItemText(const wxString & text);
wxString GetCharsetMenuItemText();

wxInt32 GetCurrentLang();

wxString RemoveUrlEncoding(const wxString & str);
wxString AddUrlEncoding(const wxString & str);
