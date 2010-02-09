#pragma once

class CReadBookDC :
    public wxClientDC
{
public:
    CReadBookDC(wxWindow * pWindow);
    virtual ~CReadBookDC(void);

private:
    wxFont m_OldFont;
    wxColour m_OldTextColor;
    wxBrush m_OldBkColor;
};
