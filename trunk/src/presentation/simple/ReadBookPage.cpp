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

#include "../../ReadBookApp.h"
#include "ReadBookPage.h"
#include "../ReadBookView.h"
#include "ObjectCache.h"
#include <vector>

CReadBookPage::CReadBookPage(CReadBookView * pView, 
							 wxInt32 nMaxLineCount,
							 wxInt32 nLineWidth,
							 wxInt32 nLineHeight,
							 wxInt32 nLineMargin,
							 wxInt32 nMaxAsciiCharCount,
							 wxInt32 nAvgAsciiCharWidth,
							 wxInt32 nColMargin) :
m_pView(pView),
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
			ReleaseReadBookLine(m_ReadBookLines[i]);
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

	pLine = CreateReadBookLine(m_pView,
		m_nLineWidth, 
		m_nMaxAsciiCharCount, 
		m_nAvgAsciiCharWidth, 
		m_nColMargin);

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

	if (destroy) ReleaseReadBookLine(pLine);

	m_ReadBookLines[nRow] = NULL;

	return true;
}

void CReadBookPage::TrimEmptyLines(bool empty_after_data)
{
	wxInt32 nBeginLine = -1;

	if (empty_after_data)
	{
		for(wxInt32 i = 0; i < (wxInt32)m_nMaxLineCount; i++)
		{
			CReadBookLine * pLine = m_ReadBookLines[i];

			if (pLine == NULL && nBeginLine < 0)
			{
				nBeginLine = i;
			}

			if (pLine != NULL && nBeginLine >= 0)
			{
				m_ReadBookLines[nBeginLine++] = pLine;
			}
		}

		if (nBeginLine >= 0)
		{
			for(wxInt32 i = nBeginLine; i < (wxInt32)m_nMaxLineCount; i++)
				m_ReadBookLines[i] = NULL;
		}
	}
	else
	{
		for(wxInt32 i = (wxInt32)m_nMaxLineCount - 1; i >= 0; i--)
		{
			CReadBookLine * pLine = m_ReadBookLines[i];

			if (pLine == NULL && nBeginLine < 0)
			{
				nBeginLine = i;
			}

			if (pLine != NULL && nBeginLine >= 0)
			{
				m_ReadBookLines[nBeginLine--] = pLine;
			}
		}

		if (nBeginLine >= 0)
		{
			for(wxInt32 i = 0; i <= nBeginLine; i++)
				m_ReadBookLines[i] = NULL;
		}
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

void CReadBookPage::Clear()
{
	Clear(0, m_nMaxLineCount - 1);
}

void CReadBookPage::Clear(wxInt32 nBeginRow, wxInt32 nEndRow)
{
	for(wxInt32 i = nBeginRow; i <= nEndRow; i++)
	{
		if (m_ReadBookLines[i] != NULL)
			ReleaseReadBookLine(m_ReadBookLines[i]);

		m_ReadBookLines[i] = NULL;
	}
}

void CReadBookPage::StripHtml(wxDC * pDC, 
							  wxInt32 nBeginRow, wxInt32 nEndRow,
							  int & initial_tag_status,
							  int & initial_script_status)
{
	wxString tag = wxT("");
	bool bInHtmlTag = initial_tag_status != 0;
	bool bInScript = initial_script_status != 0;

	std::vector<CReadBookChar *> chars;
	std::vector<CReadBookChar *>::iterator it;

	for(wxInt32 row = nBeginRow; row <= nEndRow; row++)
	{
		CReadBookLine * pLine = m_ReadBookLines[row];

		if (pLine != NULL)
		{
			wxInt32 nCharCount = pLine->GetAsciiCharCount();

			for(wxInt32 col = 0; col < nCharCount; col++)
			{
				CReadBookChar * pChar = pLine->GetChar(col);

				if (pChar == NULL)
					continue;

				if (pChar->GetChar() == '<')
				{
					bInHtmlTag = true;
					initial_tag_status = false;
					tag.Clear();
				}
				else if (pChar->GetChar() == '>')
				{
					if (tag.Length() == 0 && !bInHtmlTag)
					{
						for(it = chars.begin(); it != chars.end(); it++)
						{
							if (NULL != *it)
								ReleaseReadBookChar(*it);
						}

						initial_tag_status = true;
						chars.clear();
					}
					else if (tag.CmpNoCase(wxT("p")) == 0 ||
						tag.CmpNoCase(wxT("br")) == 0)
					{
						//Create a new line
						chars.push_back((CReadBookChar *)NULL);
					}
					//TODO: how to handle script correctly, is a big problem?
					//else if (tag.MakeLower().StartsWith(wxT("script ")))
					//{
					//	//bInScript = true;
					//	//initial_script_status = false;
					//}
					//else if (tag.CmpNoCase(wxT("/script")) == 0)
					//{
					//	//if (!bInScript)
					//	//	initial_script_status = true;

					//	//bInScript = false;
					//}
					//else if (tag.CmpNoCase(wxT("!--")) == 0)
					//{
					//	bInScript = true;
					//}
					//else if (tag.CmpNoCase(wxT("--")) == 0)
					//{
					//	bInScript = false;
					//}

					bInHtmlTag = false;
				}
				else if (!bInHtmlTag)
				{
					RemoveChar(row, col, false);

					chars.push_back(pChar);
				}
				else
				{
					tag.Append(pChar->GetChar());
				}

				if (!pChar->IsAscii())
					col++;
			}//for each char

			chars.push_back(NULL);
		}//each line

	}//for line

	Clear(nBeginRow, nEndRow);

	if (chars.size() > 0)
	{
		it = chars.begin();

		for(wxInt32 row = nBeginRow; row <= nEndRow; row++)
		{
			if (it == chars.end())
				break;

			wxInt32 col = 0;
			CReadBookChar * pChar = *it;

			while(true)
			{
				if (pChar == NULL)
				{
					it++;
					break;
				}

				if (!SetChar(row, col++, pChar, pDC))
				{
					break;
				}

				it++;

				if (it == chars.end())
					break;

				if (!pChar->IsAscii())
					col++;

				pChar = *it;
			}
		}//for row

		for(;it != chars.end(); it++)
		{
			if (*it != NULL)
			{
				ReleaseReadBookChar(*it);
			}
		}

		TrimEmptyLines();
	}//if chars

	if (bInHtmlTag)
		initial_tag_status = bInHtmlTag;

	if (bInScript)
		initial_script_status = bInScript;
}
