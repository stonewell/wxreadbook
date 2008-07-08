// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/dir.h"
#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "wx/filesys.h"
#include "wx/fs_arc.h"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "ReadBookApp.h"
#include "ReadBookMainFrm.h"
#include "ReadBookBufferedDoc.h"

#include <fstream>

IMPLEMENT_DYNAMIC_CLASS(CReadBookBufferedDoc, CReadBookDoc)

#define BUFFER_SIZE (200)
#define BUFFER_MOVE_DELTA (BUFFER_SIZE / 2)

static wxInt32 VALID_UNICODE[][2] =
{
	{0x0000, 0x007F},
	{0x0080, 0x00FF},
	{0x3000, 0x303F},
	{0x2000, 0x206F},
	{0x2E00, 0x2E7F},
	{0xFF00, 0xFFEF},
	{0x20A0, 0x20CF},
	{0x4E00, 0x9FBF},
	{0x3400, 0x4DBF},
	{0x20000, 0x2A6DF},
	{0xF900, 0xFAFF},
	{0x2F800, 0x2FA1F},
	{0x3190, 0x319F},
	{0x25A0, 0x25FF},
	{0x2460, 0x24FF},
	{0x2600, 0x26FF},
	{0x3100, 0x312F},
	{0x0300, 0x036F}
};

static wxInt32 INVALID_UINICODE[][2] = 
{
	{0xE000, 0xF8FF},
	{0xF0000, 0xFFFFD},
	{0x100000, 0x10FFFD},
	{0xD800, 0xDBFF},
	{0xDC00, 0xDFF},
	{0xFFF0, 0xFFFF},
	{0x1FF80, 0x1FFFF},
	{0x2FF80, 0x2FFFF},
	{0x3FF80, 0x3FFFF},
	{0x4FF80, 0x4FFFF},
	{0x5FF80, 0x5FFFF},
	{0x6FF80, 0x6FFFF},
	{0x7FF80, 0x7FFFF},
	{0x8FF80, 0x8FFFF},
	{0x9FF80, 0x9FFFF},
	{0xAFF80, 0xAFFFF},
	{0xBFF80, 0xBFFFF},
	{0xCFF80, 0xCFFFF},
	{0xDFF80, 0xDFFFF},
	{0xEFF80, 0xEFFFF},
	{0xFFF80, 0xFFFFF},
	{0x10FF80, 0x10FFFF}
};

bool IsValidUnicode(wxChar ch)
{
	//Invalid Chars
	int size = sizeof(INVALID_UINICODE) / 2 / sizeof(wxInt32);

	for(int i = 0; i < size; i++)
	{
		if (ch >= INVALID_UINICODE[i][0] && ch <= INVALID_UINICODE[i][1])
		{
			return false;
		}
	}

	//Valid Chars
	size = sizeof(VALID_UNICODE) / 2 / sizeof(wxInt32);

	for(int i = 0; i < size; i++)
	{
		if (ch >= VALID_UNICODE[i][0] && ch <= VALID_UNICODE[i][1])
		{
			return true;
		}
	}

	return true;
}

CReadBookBufferedDoc::CReadBookBufferedDoc(void) :
m_pFile(NULL),
m_pInput(NULL),
m_pConv(NULL),
m_nLastReadRow(-1),
m_nCharSize(0),
m_nMBCharSize(0),
m_nLineSize(0)
{
}

CReadBookBufferedDoc::~CReadBookBufferedDoc(void)
{
	CleanUp();
}

const wxString & EMPTY_STRING = wxT("");
const wxString & CReadBookBufferedDoc::GetLine(wxInt32 nRow)
{
	if (m_pInput == NULL || nRow < 0)
		return EMPTY_STRING;

	if (m_LinesMapping.find(nRow) != m_LinesMapping.end())
	{
		wxFileOffset offset = GetRowOffset(nRow + 1);

		if (offset >= 0)
		{
			m_pInput->SeekI(offset);

			m_nLastReadRow = nRow;

			return m_LinesMapping[nRow];
		}

		offset = GetRowOffset(nRow);

		m_pInput->SeekI(offset);

		m_nLastReadRow = nRow - 1;
	}

	if (!(nRow == m_nLastReadRow + 1 && m_nLastReadRow >= 0))
	{
		wxFileOffset offset = RowToOffset(nRow);

		m_pInput->SeekI(offset);
	}

	wxInt32 nLineWidth = m_nLineSize;

	wxString strLine = wxT("");
	wxInt32 nReadedWidth = 0;

	wxInt32 saved_ReadedWidth = nReadedWidth;
	wxInt32 read_loop = 0;
	wxFileOffset offset = m_pInput->TellI();

	bool filter_invalid_char = false;
	do
	{
		bool error = false;

		wxInt32 saved_LineWidth = nLineWidth;

		for(wxInt32 len = 0; len < 9 ; len ++)
		{
			nLineWidth = saved_LineWidth;

			strLine = wxT("");
			error = false;

			if (offset - len >= 0)
			{
				m_pInput->SeekI(offset - len);
				//nLineWidth += len;
				nReadedWidth = saved_ReadedWidth;

				if (nRow == GetCurrentLine())
				{
					wxGetApp().GetPreference()->SetFileInfo(GetFileName(), 
						GetCurrentLine(), offset - len);
				}

				m_RowOffsetMap[nRow] = offset - len;
			}
			else
			{
				break;
			}

			for(;nReadedWidth<nLineWidth;)
			{
				wxChar ch = NextChar();

				if (ch == wxEOT)
				{
					error = true;
					break;
				}

				if (EatEOL(ch))
					break;

				if (!IsValidUnicode(ch))
				{
					if (!filter_invalid_char)
					{
						error = true;
						break;
					}
					else
					{
						ch = ' ';
					}
				}

				size_t byteCount = 0;
				while(m_lastBytes[byteCount])
					byteCount++;

				if (byteCount == 1)
				{
					nReadedWidth += m_nCharSize;
				}
				else
				{
					nReadedWidth += m_nMBCharSize;
				}

				if (nReadedWidth > nLineWidth)
				{
					UngetLast();
				}
				else
				{
					if (ch == '\t')
					{
						strLine.Append(wxT("    "));
					}
					else
					{
						strLine.Append(ch);
					}
				}
			}

			if (!error || m_pInput->Eof())
			{
				break;
			}
		}//for len

		if (error)
		{
			filter_invalid_char = true;
		}
		else if (read_loop > 0)
		{
			if (IsEmptyLine(strLine) && !m_pInput->Eof())
			{
				filter_invalid_char = true;
			}
		}

		if (!error && IsEmptyLine(strLine))
		{
			offset = m_pInput->TellI();
		}

		read_loop ++;
	}
	while (IsEmptyLine(strLine) && !m_pInput->Eof());

	m_nLastReadRow = nRow;

	SaveLine(m_nLastReadRow, strLine);

	return m_LinesMapping[nRow];
}

wxFileOffset CReadBookBufferedDoc::RowToOffset(wxInt32 nRow)
{
	wxFileOffset offset = GetRowOffset(nRow);
		
	if (offset < 0)
	{
		wxInt32 nNearRow = FindNearRow(nRow);

		if (nNearRow >= 0)
		{
			wxFileOffset nNearOffset = GetRowOffset(nNearRow);

			offset = nNearOffset + (nRow - nNearRow) * m_nCharsPerLine;
		}
		else
		{
			offset = (wxFileOffset)m_nCharsPerLine * nRow;
		}
	}

	if (offset >= m_nFileLength)
		offset = m_nFileLength;

	if (offset < 0)
		offset = 0;

	return offset;
}

wxInt32 CReadBookBufferedDoc::FindNearRow(wxInt32 nRow)
{
	wxInt32 minRow = -1;
	wxInt32 maxRow = m_nFileLength;

	wxMapFileOffset::iterator it = m_RowOffsetMap.begin();

	for(;it != m_RowOffsetMap.end();it++)
	{
		if (it->first < nRow && it->first > minRow)
		{
			minRow = it->first;
		}

		if (it->first > nRow && it->first < maxRow)
		{
			maxRow = it->first;
		}
	}

	if (minRow == -1 && maxRow == m_nFileLength)
	{
		return -1;
	}

	if (minRow == -1)
		return maxRow;

	if (maxRow == m_nFileLength)
		return minRow;

	if (nRow - minRow < maxRow - nRow)
		return minRow;

	return maxRow;
}

void CReadBookBufferedDoc::SaveLine(wxInt32 nRow, const wxString & strLine)
{
	while(m_LinesMapping.size() >= BUFFER_SIZE)
	{
		CIntStringMap::iterator it;

		wxInt32 row = m_LinesMapping.begin()->first;

		for(it = m_LinesMapping.begin(); it != m_LinesMapping.end(); it++)
		{
			if (it->first < row)
			{
				row = it->first;
			}
		}

		m_LinesMapping.erase(row);
	}

	m_LinesMapping[nRow] = strLine;

	wxArrayInt results;

	for(CIntStringMap::iterator it = m_LinesMapping.begin(); it != m_LinesMapping.end(); it++)
	{
		if (it->first > nRow)
		{
			results.Add(it->first);
		}
	}

	for(size_t i = 0;i < results.GetCount(); i++)
		m_LinesMapping.erase(results[i]);
}

bool CReadBookBufferedDoc::LoadBuffer(const wxString & url, wxMBConv * conv, bool bGuess)
{
	CleanUp();

	m_FileSystem.ChangePathTo(url, false);

	m_pFile = m_FileSystem.OpenFile(url, wxFS_READ | wxFS_SEEKABLE);

	if (m_pFile == NULL)
		return false;

	m_pInput = m_pFile->GetStream();

	m_pConv = GetSuitableMBConv(m_pInput, conv, bGuess);

	m_pInput->SeekI(0, wxFromEnd);
	m_nFileLength = m_pInput->TellI();
	m_pInput->SeekI(0, wxFromStart);

	CFileInfo * pFileInfo = NULL;

	wxInt32 nRow = wxGetApp().GetPreference()->GetFileInfo(url, &pFileInfo);

	if (pFileInfo != NULL)
	{
		m_RowOffsetMap[nRow] = pFileInfo->m_nFilePos;

		m_pInput->SeekI(pFileInfo->m_nFilePos);
	}

	return true;
}

void CReadBookBufferedDoc::CleanUp()
{
	if (m_pFile != NULL)
	{
		delete m_pFile;
		m_pFile = NULL;
	}

	m_pInput = NULL;

	m_LinesMapping.clear();
	m_nLastReadRow = -1;
	m_RowOffsetMap.clear();
}

wxInt32 CReadBookBufferedDoc::GetBufferSize(void) const
{
	return m_nFileLength;
}

void CReadBookBufferedDoc::SetCharsPerLine(wxInt32 charsPerLine)
{
	m_nCharsPerLine = charsPerLine;

	m_LinesMapping.clear();
}

wxChar CReadBookBufferedDoc::NextChar()
{
	if (m_pInput == NULL)
		return wxEOT;

#if wxUSE_UNICODE
	wxChar wbuf[2];

	memset((void*)m_lastBytes, 0, 10);

	for(size_t inlen = 0; inlen < 9; inlen++)
	{
		// actually read the next character
		m_lastBytes[inlen] = m_pInput->GetC();

		if(m_pInput->LastRead() <= 0)
			return wxEOT;

		if ( m_pConv->ToWChar(wbuf, WXSIZEOF(wbuf), m_lastBytes, inlen + 1)
			!= wxCONV_FAILED )
		{
			return wbuf[0];
		}
	}

	// there should be no encoding which requires more than nine bytes for one character...
	return wxEOT;
#else
	m_lastBytes[0] = m_pInput->GetC();

	if(m_pInput->LastRead() <= 0)
		return wxEOT;

	return m_lastBytes[0];
#endif

}

void CReadBookBufferedDoc::UngetLast()
{
	if (m_pInput == NULL)
		return;

	size_t byteCount = 0;
	while(m_lastBytes[byteCount]) // pseudo ANSI strlen (even for Unicode!)
		byteCount++;
	m_pInput->Ungetch(m_lastBytes, byteCount);
	memset((void*)m_lastBytes, 0, 10);
}

bool CReadBookBufferedDoc::EatEOL(const wxChar &c)
{
	if (c == wxT('\n')) return true; // eat on UNIX

	if (c == wxT('\r')) // eat on both Mac and DOS
	{
		wxChar c2 = NextChar();
		if(c2 == wxEOT) return true; // end of stream reached, had enough :-)

		if (c2 != wxT('\n')) UngetLast(); // Don't eat on Mac
		return true;
	}

	return false;
}

void CReadBookBufferedDoc::ShiftStream(wxInt32 delta)
{
	if (m_pInput == NULL)
		return;

	wxFileOffset offset = (wxFileOffset)m_nCharsPerLine * GetCurrentLine();

	if (offset >= m_nFileLength)
		offset = m_nFileLength - m_nCharsPerLine;

	m_pInput->SeekI(offset + delta);

	m_nLastReadRow = GetCurrentLine() - 1;
	m_LinesMapping.clear();
}

wxFileOffset CReadBookBufferedDoc::GetRowOffset(wxInt32 nRow)
{
	wxMapFileOffset::iterator it = m_RowOffsetMap.find(nRow);

	if (it == m_RowOffsetMap.end())
		return -1;

	return it->second;
}

wxInt32 CReadBookBufferedDoc::GetBufferCurPos(void)
{
	if (m_pInput != NULL)
		return m_pInput->TellI();

	return 0;
}

wxInt32 CReadBookBufferedDoc::OffsetToRow(wxInt32 nOffset)
{
	wxArrayFileOffset sortedArray;

	wxMapFileOffset::iterator it = m_RowOffsetMap.begin();

	for(;it != m_RowOffsetMap.end();it++)
	{
		bool bFound = false;

		wxArrayFileOffset::iterator itSort = sortedArray.begin();

		for(; itSort != sortedArray.end(); itSort++)
		{
			if (*itSort > it->second)
			{
				sortedArray.insert(itSort, it->second);
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			sortedArray.push_back(it->second);
		}
	}

	wxArrayFileOffset::iterator itSort = sortedArray.begin();

	bool bFound = false;

	wxFileOffset findOffset = 0;

	for(; itSort != sortedArray.end(); itSort++)
	{
		if (*itSort > nOffset)
		{
			if (itSort == sortedArray.begin())
			{
				findOffset = *itSort;
			}
			else
			{
				findOffset = *(itSort - 1);
			}
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		if (sortedArray.size() > 0)
		{
			wxFileOffset maxOff = *(sortedArray.end() - 1);

			wxInt32 nRow = GetOffsetRow(maxOff);

			if (nRow >= 0)
			{
				return nRow + (nOffset - maxOff) / m_nCharsPerLine + 1;
			}
		}

		return nOffset / m_nCharsPerLine + 1;
	}

	return GetOffsetRow(findOffset);
}

wxInt32 CReadBookBufferedDoc::GetOffsetRow(wxFileOffset nOffset)
{
	wxMapFileOffset::iterator it = m_RowOffsetMap.begin();

	for(;it != m_RowOffsetMap.end();it++)
	{
		if (it->second == nOffset)
			return it->first;
	}

	return -1;
}
