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

#include "ReadBookApp.h"
#include "ReadBookLine.h"

CReadBookLine::CReadBookLine(wxUint32 nMaxAsciiCharCount,
        wxUint32 nAvgAsciiCharWidth,
        wxUint32 nColMargin) :
m_nMaxAsciiCharCount(nMaxAsciiCharCount),
m_nAvgAsciiCharWidth(nAvgAsciiCharWidth),
m_nColMargin(nColMargin)
{
    m_ReadBookChars = new CReadBookChar * [ m_nMaxAsciiCharCount ];

    memset(m_ReadBookChars, 0, m_nMaxAsciiCharCount * sizeof(CReadBookChar *));
}

CReadBookLine::~CReadBookLine(void)
{
	for(wxUint32 i = 0; i < m_nMaxAsciiCharCount; i++)
	{
        delete m_ReadBookChars[i];
	}

    delete m_ReadBookChars;
}

bool CReadBookLine::SetChar(wxUint32 nPos, const CReadBookChar * pChar)
{
    if (nPos >= m_nMaxAsciiCharCount)
        return false;

    if (GetChar(nPos) != NULL)
        return false;

    if (!pChar->IsAscii() && GetChar(nPos + 1) != NULL)
        return false;

    m_ReadBookChars[nPos] = const_cast<CReadBookChar *>(pChar);

    if (!pChar->IsAscii())
        m_ReadBookChars[nPos + 1] = m_ReadBookChars[nPos];

    return true;
}

bool CReadBookLine::AppendChar(const CReadBookChar * pChar)
{
    return SetChar(GetAsciiCharCount(), pChar);
}

bool CReadBookLine::RemoveChar(wxUint32 nPos,bool destroy)
{
    CReadBookChar * pChar = GetChar(nPos);

    if (pChar == NULL) return true;

    m_ReadBookChars[nPos] = NULL;

    if (nPos > 0)
    {
        if (m_ReadBookChars[nPos - 1] == pChar)
            m_ReadBookChars[nPos - 1] = NULL;
    }

    if (nPos < m_nMaxAsciiCharCount - 1)
    {
        if (m_ReadBookChars[nPos + 1] == pChar)
            m_ReadBookChars[nPos + 1] = NULL;
    }

    if (destroy) delete pChar;

    return true;
}

CReadBookChar * CReadBookLine::GetChar(wxUint32 nPos)
{
    if (nPos < m_nMaxAsciiCharCount)
        return m_ReadBookChars[nPos];

    return NULL;
}

wxFileOffset CReadBookLine::GetFileOffset() const
{
	for(wxUint32 i = 0; i < m_nMaxAsciiCharCount; i++)
    {
        CReadBookChar * pChar = m_ReadBookChars[i];

        if (pChar != NULL)
            return pChar->GetFileOffset();
    }

    return 0;
}

void CReadBookLine::Paint(wxInt32 x, wxInt32 y, wxDC * pDC, wxInt32 count)
{
    if (count < 0) count = m_nMaxAsciiCharCount;

    for(wxInt32 i = 0; i < count; i++)
    {
        CReadBookChar * pChar = m_ReadBookChars[i];

        if (pChar != NULL)
        {
            pChar->Paint(x, y, pDC);

            if (!pChar->IsAscii())
            {
                x += m_nAvgAsciiCharWidth;
                x += m_nColMargin;

                i++;
            }
        }

        x += m_nAvgAsciiCharWidth;
        x += m_nColMargin;
    }
}

wxUint32 CReadBookLine::GetAsciiCharCount() const
{
    for(wxUint32 i = m_nMaxAsciiCharCount; i >= 0; i--)
    {
        if (m_ReadBookChars[i] != NULL)
            return i + 1;

        if (i == 0)
            break;
    }

    return 0;
}