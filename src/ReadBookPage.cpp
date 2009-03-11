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

CReadBookPage::CReadBookPage(wxInt32 nMaxLineCount,
        wxInt32 nLineWidth,
        wxInt32 nLineHeight,
        wxInt32 nLineMargin,
        wxInt32 nMaxAsciiCharCount,
        wxInt32 nAvgAsciiCharWidth,
        wxInt32 nColMargin) :
m_nMaxLineCount(nMaxLineCount),
m_nLineWidth(nLineWidth),
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
    for(wxInt32 i = 0; i < (wxInt32)m_nMaxLineCount; i++)
    {
        if (m_ReadBookLines[i] != NULL)
            delete m_ReadBookLines[i];
    }

    delete m_ReadBookLines;
}

bool CReadBookPage::SetChar(wxInt32 nRow, wxInt32 nCol, const CReadBookChar * pChar, wxDC * pDC)
{
    CReadBookLine * pLine = GetLine(nRow);

    if (pLine == NULL)
    {
        pLine = CreateLine(nRow);
    }

    if (pLine == NULL)
        return false;

    return pLine->SetChar(nCol, pChar,pDC);
}

bool CReadBookPage::AppendChar(const CReadBookChar * pChar, wxDC * pDC)
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

    return pLine->AppendChar(pChar, pDC);
}

bool CReadBookPage::RemoveChar(wxInt32 nRow, wxInt32 nCol, bool destroy)
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

    for(wxInt32 i = m_nMaxLineCount - 1; i >= 0; i--)
    {
        if (m_ReadBookLines[i] != NULL)
            return i + 1;

        if (i == 0)
            break;
    }

    return 0;
}

CReadBookLine * CReadBookPage::GetLine(wxInt32 nRow) const
{
    if (m_nMaxLineCount > nRow)
        return m_ReadBookLines[nRow];

    return NULL;
}

CReadBookChar * CReadBookPage::GetChar(wxInt32 nRow, wxInt32 nCol) const
{
    CReadBookLine * pLine = GetLine(nRow);

    if (pLine == NULL) return NULL;

    return pLine->GetChar(nCol);
}

wxFileOffset CReadBookPage::GetBeginFileOffset() const
{
	for(wxInt32 i = 0; i < (wxInt32)m_nMaxLineCount; i++)
    {
        CReadBookLine * pLine = m_ReadBookLines[i];

        if (pLine != NULL)
            return pLine->GetBeginFileOffset();
    }

    return 0;
}

wxFileOffset CReadBookPage::GetEndFileOffset() const
{
	for(wxInt32 i = m_nMaxLineCount - 1; i >= 0; i--)
    {
        CReadBookLine * pLine = m_ReadBookLines[i];

        if (pLine != NULL)
            return pLine->GetEndFileOffset();
    }

    return 0;
}

void CReadBookPage::Paint(wxInt32 x, wxInt32 y, wxDC * pDC)
{
	for(wxInt32 i = 0; i < (wxInt32)m_nMaxLineCount; i++)
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

CReadBookLine * CReadBookPage::CreateLine(wxInt32 nRow)
{
    CReadBookLine * pLine = GetLine(nRow);

    if (pLine != NULL) return pLine;

    if (nRow >= m_nMaxLineCount) return NULL;

    pLine = new CReadBookLine(m_nLineWidth, m_nMaxAsciiCharCount, m_nAvgAsciiCharWidth, m_nColMargin);

    m_ReadBookLines[nRow] = pLine;

    return pLine;
}

bool CReadBookPage::SetLine(wxInt32 nRow, const CReadBookLine * pLine)
{
    if (NULL != GetLine(nRow))
    {
        return false;
    }

    m_ReadBookLines[nRow] = const_cast<CReadBookLine *>(pLine);

    return true;
}

bool CReadBookPage::RemoveLine(wxInt32 nRow, bool destroy)
{
    CReadBookLine * pLine = GetLine(nRow);

    if (NULL == pLine)
        return true;

    if (destroy) delete pLine;

    m_ReadBookLines[nRow] = NULL;

    return true;
}

void CReadBookPage::TrimEmptyLines()
{
    wxInt32 nBeginLine = -1;
    wxInt32 nLastLine = -1;

	for(wxInt32 i = 0; i < (wxInt32)m_nMaxLineCount; i++)
    {
        CReadBookLine * pLine = m_ReadBookLines[i];

        if (pLine != NULL && nBeginLine < 0)
        {
            nBeginLine = i;
        }

        if (nBeginLine == 0)
            return;

        if (nBeginLine > 0)
        {
            m_ReadBookLines[i - nBeginLine] = pLine;
            nLastLine = i - nBeginLine;
        }
    }

    if (nLastLine >= 0)
    {
        for(wxInt32 i = nLastLine + 1; i < (wxInt32)m_nMaxLineCount; i++)
            m_ReadBookLines[i] = NULL;
    }
}

void CReadBookPage::NormalizeLines()
{
	for(wxInt32 i = 0; i < (wxInt32)m_nMaxLineCount; i++)
    {
        if (m_ReadBookLines[i] != NULL)
            m_ReadBookLines[i]->TrimEmptyChars();
    }
}
