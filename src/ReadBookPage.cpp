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
#include "ReadBookPage.h"

CReadBookPage::CReadBookPage(wxUint32 nMaxLineCount,
        wxUint32 nLineHeight,
        wxUint32 nLineMargin,
        wxUint32 nMaxAsciiCharCount,
        wxUint32 nAvgAsciiCharWidth,
        wxUint32 nColMargin) :
m_nMaxLineCount(nMaxLineCount),
m_nLineHeight(nLineHeight),
m_nLineMargin(nLineMargin),
m_nMaxAsciiCharCount(nMaxAsciiCharCount),
m_nAvgAsciiCharWidth(nAvgAsciiCharWidth),
m_nColMargin(nColMargin)
{
    m_ReadBookLines = new CReadBookLine *[nMaxLineCount];

    memset(m_ReadBookLines, 0, nMaxLineCount * sizeof(CReadBookLine *));
}

CReadBookPage::~CReadBookPage(void)
{
    for(wxUint32 i = 0; i < m_nMaxLineCount; i++)
    {
        if (m_ReadBookLines[i] != NULL)
            delete m_ReadBookLines[i];
    }

    delete m_ReadBookLines;
}

bool CReadBookPage::SetChar(wxUint32 nRow, wxUint32 nCol, const CReadBookChar * pChar)
{
    CReadBookLine * pLine = GetLine(nRow);

    if (pLine == NULL)
    {
        pLine = CreateLine(nRow);
    }

    if (pLine == NULL)
        return false;

    return pLine->SetChar(nCol, pChar);
}

bool CReadBookPage::AppendChar(const CReadBookChar * pChar)
{
    wxInt32 nLineCount = GetLineCount();

    CReadBookLine * pLine = NULL;

    if (nLineCount == 0)
    {
        pLine = CreateLine(0);
    }
    else
    {
        pLine = GetLine(nLineCount - 1);
    }

    if (pLine == NULL)
        return false;

    return pLine->AppendChar(pChar);
}

bool CReadBookPage::RemoveChar(wxUint32 nRow, wxUint32 nCol, bool destroy)
{
    CReadBookLine * pLine = GetLine(nRow);

    if (pLine == NULL)
        return false;

    return pLine->RemoveChar(nCol,destroy);
}

wxInt32 CReadBookPage::GetLineCount() const
{
    if (m_nMaxLineCount == 0)
        return 0;

    for(wxUint32 i = m_nMaxLineCount - 1; i >= 0; i--)
    {
        if (m_ReadBookLines[i] != NULL)
            return i + 1;

        if (i == 0)
            break;
    }

    return 0;
}

CReadBookLine * CReadBookPage::GetLine(wxUint32 nRow) const
{
    if (m_nMaxLineCount > nRow)
        return m_ReadBookLines[nRow];

    return NULL;
}

CReadBookChar * CReadBookPage::GetChar(wxUint32 nRow, wxUint32 nCol) const
{
    CReadBookLine * pLine = GetLine(nRow);

    if (pLine == NULL) return NULL;

    return pLine->GetChar(nCol);
}

wxFileOffset CReadBookPage::GetFileOffset() const
{
	for(wxUint32 i = 0; i < m_nMaxLineCount; i++)
    {
        CReadBookLine * pLine = m_ReadBookLines[i];

        if (pLine != NULL)
            return pLine->GetFileOffset();
    }

    return 0;
}

void CReadBookPage::Paint(wxInt32 x, wxInt32 y, wxDC * pDC)
{
	for(wxUint32 i = 0; i < m_nMaxLineCount; i++)
	{
        wxInt32 nRow = i;
        CReadBookLine * pLine = m_ReadBookLines[i];

        if (pLine != NULL)
        {
            wxInt32 nLineY = nRow * (m_nLineHeight + m_nLineMargin) + y;
            wxInt32 nLineX = x;

            pLine->Paint(nLineX, nLineY, pDC);
        }
	}
}

CReadBookLine * CReadBookPage::CreateLine(wxUint32 nRow)
{
    CReadBookLine * pLine = GetLine(nRow);

    if (pLine != NULL) return pLine;

    if (nRow >= m_nMaxLineCount) return NULL;

    pLine = new CReadBookLine(m_nMaxAsciiCharCount, m_nAvgAsciiCharWidth, m_nColMargin);

    m_ReadBookLines[nRow] = pLine;

    return pLine;
}

bool CReadBookPage::SetLine(wxUint32 nRow, const CReadBookLine * pLine)
{
    if (NULL != GetLine(nRow))
    {
        return false;
    }

    m_ReadBookLines[nRow] = const_cast<CReadBookLine *>(pLine);

    return true;
}

bool CReadBookPage::RemoveLine(wxUint32 nRow, bool destroy)
{
    CReadBookLine * pLine = GetLine(nRow);

    if (NULL == pLine)
        return true;

    if (destroy) delete pLine;

    m_ReadBookLines[nRow] = NULL;

    return true;
}

