// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/hashmap.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
#endif

#include "wx/datstrm.h"
#include "wx/mstream.h"
#include "wx/ptr_scpd.h"
#include "wx/wfstream.h"

#include "7ZipEntry.h"
#include "7ZipOutputStream.h"
#include "7ZipInputStream.h"
#include "7ZipClassFactory.h"

wxString g_CurrentUrl = wxT("");

// constructor
//
C7ZipInputStream::C7ZipInputStream(wxInputStream& stream,
                                   wxMBConv& conv /*=wxConvLocal*/)
  : wxArchiveInputStream(stream, conv)
{
    Init();
}

C7ZipInputStream::C7ZipInputStream(wxInputStream *stream,
                                   wxMBConv& conv /*=wxConvLocal*/)
  : wxArchiveInputStream(stream, conv)
{
    Init();
}

void C7ZipInputStream::Init()
{
	m_nCurrentEntryIndex = 0;
	m_p7ZipArchive = NULL;
	
	m_7ZipLibrary.Initialize();

	if (m_7ZipLibrary.OpenArchive(this, &m_p7ZipArchive))
	{
		unsigned int numItems = 0;
		if (m_p7ZipArchive->GetItemCount(&numItems))
		{
			for(unsigned int i = 0;i<numItems;i++)
			{
				C7ZipArchiveItem * pArchiveItem = NULL;

				if (m_p7ZipArchive->GetItemInfo(i, &pArchiveItem))
				{
					C7ZipEntry * pEntry = new C7ZipEntry(pArchiveItem->GetFullPath().c_str());

					pEntry->SetIsDir(pArchiveItem->IsDir());
					pEntry->SetExternalAttributes(pArchiveItem->GetArchiveIndex());

					m_Entries.push_back(pEntry);
				}
			}
		}
	}
}

C7ZipInputStream::~C7ZipInputStream()
{
	if (m_p7ZipArchive != NULL)
	{
		m_p7ZipArchive->Close();
		delete m_p7ZipArchive;
	}

	m_7ZipLibrary.Deinitialize();

	m_Entries.clear();
}

// Can be overriden to add support for additional decompression methods
//
wxInputStream *C7ZipInputStream::OpenDecompressor(wxInputStream& stream)
{
    return NULL;
}

bool C7ZipInputStream::CloseDecompressor(wxInputStream *decomp)
{
    return true;
}

size_t C7ZipInputStream::OnSysRead(void *buffer, size_t size)
{
	return m_parent_i_stream->Read(buffer, size).LastRead();
}

C7ZipEntry *C7ZipInputStream::GetNextEntry()
{
	if (m_nCurrentEntryIndex < m_Entries.size())
	{
		return m_Entries[m_nCurrentEntryIndex++];
	}

	return NULL;
}

bool C7ZipInputStream::OpenEntry(wxArchiveEntry& entry)
{
	return false;
}

bool C7ZipInputStream::CloseEntry()
{
	return false;
}

wstring C7ZipInputStream::GetFullPath() const
{
	return g_CurrentUrl.c_str();
}

int C7ZipInputStream::Read(void *data, unsigned int size, unsigned int *processedSize)
{
	wxArchiveInputStream::Read(data, size);

	*processedSize = LastRead();

	return 0;
}

int C7ZipInputStream::Seek(__int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition)
{
	wxSeekMode mode = wxFromStart;

	switch(seekOrigin)
	{
	case FILE_CURRENT:
		mode = wxFromCurrent;
		break;
	case FILE_BEGIN:
		mode = wxFromStart;
		break;
	case FILE_END:
		mode = wxFromEnd;
		break;
	default:
		break;
	}

	if (this->IsSeekable())
	{
		SeekI(offset,mode);
		if (newPosition != NULL)
		{
			*newPosition = TellI();
		}
	}

	return 0;
}

int C7ZipInputStream::GetSize(unsigned __int64 * size)
{
	*size = GetLength();

	return 0;
}

wxFileOffset C7ZipInputStream::OnSysSeek(wxFileOffset seek, wxSeekMode mode)
{
	return m_parent_i_stream->SeekI(seek, mode);
}

wxFileOffset C7ZipInputStream::OnSysTell() const
{
	return m_parent_i_stream->TellI();
}

