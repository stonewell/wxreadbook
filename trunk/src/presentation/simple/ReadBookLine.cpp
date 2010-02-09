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
#include "ReadBookLine.h"
#include "../ReadBookView.h"
#include "ObjectCache.h"

CReadBookLine::CReadBookLine(CReadBookView * pView,
							 wxInt32 nLineWidth,
							 wxInt32 nMaxAsciiCharCount,
							 wxInt32 nAvgAsciiCharWidth,
							 wxInt32 nColMargin) :
m_pView(NULL),
m_nLineWidth(0),
m_nMaxAsciiCharCount(0),
m_nAvgAsciiCharWidth(0),
m_nColMargin(0),
m_ReadBookChars(NULL)
{
	Init(pView, nLineWidth, nMaxAsciiCharCount, nAvgAsciiCharWidth, nColMargin);
}

CReadBookLine::~CReadBookLine(void)
{
	Release();
}

bool CReadBookLine::SetChar(wxInt32 nPos, const CReadBookChar * pChar, wxDC * pDC)
{
	if (nPos >= m_nMaxAsciiCharCount)
		return false;

	if (!pChar->IsAscii() && nPos + 1 >= m_nMaxAsciiCharCount)
		return false;

	if (GetChar(nPos) != NULL)
		return false;

	if (!pChar->IsAscii() && GetChar(nPos + 1) != NULL)
		return false;

	m_ReadBookChars[nPos] = const_cast<CReadBookChar *>(pChar);

	if (!pChar->IsAscii())
		m_ReadBookChars[nPos + 1] = m_ReadBookChars[nPos];

	wxInt32 width = 0;

	wxInt32 charCount = GetAsciiCharCount();

	for(wxInt32 i = 0;i < charCount;i ++)
	{
		CReadBookChar * pTmpChar = m_ReadBookChars[i];

		if (pTmpChar == NULL)
		{
			width += pDC->GetTextExtent(L" ").GetWidth();
		}
		else
		{
			width += pDC->GetTextExtent(pTmpChar->GetChar()).GetWidth();

			if (!pTmpChar->IsAscii())
				i++;
		}
	}

	if (width > m_nLineWidth)
	{
		m_ReadBookChars[nPos] = NULL;

		if (!pChar->IsAscii())
			m_ReadBookChars[nPos + 1] = NULL;

		return false;
	}

	return true;
}

bool CReadBookLine::AppendChar(const CReadBookChar * pChar, wxDC * pDC)
{
	return SetChar(GetAsciiCharCount(), pChar, pDC);
}

bool CReadBookLine::RemoveChar(wxInt32 nPos,bool destroy)
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

	if (destroy) 
		ReleaseReadBookChar(pChar);

	return true;
}

CReadBookChar * CReadBookLine::GetChar(wxInt32 nPos)
{
	if (nPos < m_nMaxAsciiCharCount)
		return m_ReadBookChars[nPos];

	return NULL;
}

wxFileOffset CReadBookLine::GetBeginFileOffset() const
{
	for(wxInt32 i = 0; i < m_nMaxAsciiCharCount; i++)
	{
		CReadBookChar * pChar = m_ReadBookChars[i];

		if (pChar != NULL)
			return pChar->GetBeginFileOffset();
	}

	return 0;
}

wxFileOffset CReadBookLine::GetEndFileOffset() const
{
	for(wxInt32 i = m_nMaxAsciiCharCount - 1; i >= 0; i--)
	{
		CReadBookChar * pChar = m_ReadBookChars[i];

		if (pChar != NULL)
			return pChar->GetEndFileOffset();

		if (i == 0)
			break;
	}

	return 0;
}

void CReadBookLine::Paint(wxInt32 x, wxInt32 y, wxDC * pDC, wxInt32 count)
{
	if (count < 0) count = m_nMaxAsciiCharCount;

	wxInt32 spaceWidth = pDC->GetTextExtent(L" ").GetWidth();

	for(wxInt32 i = 0; i < count; i++)
	{
		CReadBookChar * pChar = m_ReadBookChars[i];

		if (pChar != NULL)
		{
			wxSize size = pChar->Paint(x, y, pDC);

			if (!pChar->IsAscii())
			{
				i++;
			}

			x += size.GetWidth();
		}
		else
		{
			x += spaceWidth;
		}
	}
}

wxInt32 CReadBookLine::GetAsciiCharCount() const
{
	for(wxInt32 i = m_nMaxAsciiCharCount - 1; i >= 0; i--)
	{
		if (m_ReadBookChars[i] != NULL)
			return i + 1;

		if (i == 0)
			break;
	}

	return 0;
}

void CReadBookLine::TrimEmptyChars()
{
	wxInt32 nBeginCol = -1;
	wxInt32 nLastCol = -1;

	for(wxInt32 i = 0; i < (wxInt32)m_nMaxAsciiCharCount; i++)
	{
		CReadBookChar * pChar = m_ReadBookChars[i];

		if (pChar != NULL && nBeginCol < 0)
		{
			nBeginCol = i;
		}

		if (nBeginCol == 0)
			return;

		if (nBeginCol > 0)
		{
			m_ReadBookChars[i - nBeginCol] = pChar;
			nLastCol = i - nBeginCol;
		}
	}

	if (nLastCol >= 0)
	{
		for(wxInt32 i = nLastCol + 1; i < (wxInt32)m_nMaxAsciiCharCount; i++)
			m_ReadBookChars[i] = NULL;
	}
}

void CReadBookLine::Init(CReadBookView * pView,
							 wxInt32 nLineWidth,
							 wxInt32 nMaxAsciiCharCount,
							 wxInt32 nAvgAsciiCharWidth,
							 wxInt32 nColMargin)
{
	if (m_nMaxAsciiCharCount != nMaxAsciiCharCount)
	{
		Release();
	
		m_nMaxAsciiCharCount = (nMaxAsciiCharCount);

		m_ReadBookChars = new CReadBookChar * [ m_nMaxAsciiCharCount ];

		memset(m_ReadBookChars, 0, m_nMaxAsciiCharCount * sizeof(CReadBookChar *));
	}
	else
	{
		Release(false);
	}

	m_pView = (pView);
	m_nLineWidth = (nLineWidth);
	m_nAvgAsciiCharWidth = (nAvgAsciiCharWidth);
	m_nColMargin = (nColMargin);
}

void CReadBookLine::Release(bool destroy)
{
	if (m_ReadBookChars == NULL)
		return;

	for(wxInt32 i = 0; i < m_nMaxAsciiCharCount; i++)
	{
		if (m_ReadBookChars[i] != NULL)
		{
			bool ascii = m_ReadBookChars[i]->IsAscii();

			ReleaseReadBookChar(m_ReadBookChars[i]);

			if (i >= m_nMaxAsciiCharCount - 1)
				break;

			if (!ascii)
				i++;
		}
	}

	if (destroy)
		delete m_ReadBookChars;
	else
		memset(m_ReadBookChars, 0, m_nMaxAsciiCharCount * sizeof(CReadBookChar *));

}
