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
#include "ReadBookSimpleDoc.h"
#include "UnicodeHelper.h"

#include <fstream>

IMPLEMENT_DYNAMIC_CLASS(CReadBookSimpleDoc, CReadBookDoc)

CReadBookSimpleDoc::CReadBookSimpleDoc(void) :
m_pFile(NULL),
m_pInput(NULL),
m_pConv(NULL)
{
}

CReadBookSimpleDoc::~CReadBookSimpleDoc(void)
{
    CleanUp();
}

bool CReadBookSimpleDoc::ReadCharAtOffset(wxFileOffset offset, 
                                          wxChar & ch, 
                                          wxFileOffset & begin_offset,
                                          wxFileOffset & end_offset,
                                          bool & end_of_line)
{
    if (m_pInput == NULL || m_pInput->Eof())
        return false;

    end_of_line = false;
    wxInt32 read_loop = 0;

    bool filter_invalid_char = false;
    do
    {
        bool error = false;

        for(wxInt32 len = 0; len < 9 ; len ++)
        {
            error = false;

            if (offset - len >= 0)
            {
                m_pInput->SeekI(offset - len);

                wxGetApp().GetPreference()->SetFileInfo(GetFileName(), 
                    GetCurrentLine(), offset - len);

                begin_offset = offset - len;
            }
            else
            {
                break;
            }

            ch = NextChar();

            while (ch == wxEOT && !m_pInput->Eof())
            {
                if (!filter_invalid_char)
                {
                    error = true;
                    break;
                }
                else
                {
                    ch = NextChar();
                }
            }

            if (!error)
            {
                if (ch == wxEOT)
                    return false;

                if (EatEOL(ch))
                {
                    end_of_line = true;
                    return true;
                }

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
            if (!filter_invalid_char && !m_pInput->Eof())
            {
                filter_invalid_char = true;
            }
            else
            {
                filter_invalid_char = false;
            }
        }
        else
        {
            filter_invalid_char = false;
        }

        read_loop ++;
    }
    while (ch == wxEOT && !m_pInput->Eof() || filter_invalid_char);

    if (ch != wxEOT)
    {
        end_offset = m_pInput->TellI();
    }

    return ch != wxEOT;
}

bool CReadBookSimpleDoc::LoadBuffer(const wxString & url, wxMBConv * conv, bool bGuess)
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
        if (pFileInfo->m_nFilePos >= 0)
        {
            m_pInput->SeekI(pFileInfo->m_nFilePos);
        }
    }

    return true;
}

void CReadBookSimpleDoc::CleanUp()
{
    if (m_pFile != NULL)
    {
        delete m_pFile;
        m_pFile = NULL;
    }

    m_pInput = NULL;
}

wxFileOffset CReadBookSimpleDoc::GetBufferSize(void) const
{
    return m_nFileLength;
}

wxChar CReadBookSimpleDoc::NextChar()
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

void CReadBookSimpleDoc::UngetLast()
{
    if (m_pInput == NULL)
        return;

    size_t byteCount = 0;
    while(m_lastBytes[byteCount]) // pseudo ANSI strlen (even for Unicode!)
        byteCount++;
    m_pInput->Ungetch(m_lastBytes, byteCount);
    memset((void*)m_lastBytes, 0, 10);
}

bool CReadBookSimpleDoc::EatEOL(const wxChar &c)
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

bool CReadBookSimpleDoc::ReadChar(wxChar & ch,         
                                  wxFileOffset & begin_offset, 
                                  wxFileOffset & end_offset, 
                                  bool & end_of_line)
{
    if (m_pInput == NULL)
        return false;

    return ReadCharAtOffset(m_pInput->TellI(), ch, begin_offset, end_offset, end_of_line);
}

void CReadBookSimpleDoc::SeekTo(wxFileOffset offset)
{
    if (m_pInput == NULL)
        return;

    m_pInput->SeekI(offset);
}