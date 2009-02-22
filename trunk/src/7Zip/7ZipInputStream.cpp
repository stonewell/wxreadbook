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

class wxTemporaryFileInputStream : public wxFileInputStream
{
public:
    wxTemporaryFileInputStream(const wxString& filename) :
        wxFileInputStream(filename), m_filename(filename) {}

    virtual ~wxTemporaryFileInputStream()
    {
        // NB: copied from wxFileInputStream dtor, we need to do it before
        //     wxRemoveFile
        if (m_file_destroy)
        {
            delete m_file;
            m_file_destroy = false;
        }

        wxRemoveFile(m_filename);
    }

protected:
    wxString m_filename;
};

class C7ZipOutStreamImpl : public virtual C7ZipOutStream
{
public:
	C7ZipOutStreamImpl(wxOutputStream * pOutStream) : m_pOutStream(pOutStream) {}
	virtual ~C7ZipOutStreamImpl() {}

	virtual int Seek(__int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition)
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

		if (m_pOutStream->IsSeekable())
		{
			m_pOutStream->SeekO(offset,mode);

			if (newPosition != NULL)
			{
				*newPosition = m_pOutStream->TellO();
			}
		}
		
		return 0;
	}

	virtual int SetSize(unsigned __int64 size)
	{
		return 0;
	}

	virtual int Write(const void *data, unsigned int size, unsigned int *processedSize)
	{
		m_pOutStream->Write(data, size);

		if (processedSize != NULL)
		{
			*processedSize = m_pOutStream->LastWrite();
		}

		return S_OK;
	}

private:
	wxOutputStream * m_pOutStream;
};

// constructor
//
C7ZipInputStream::C7ZipInputStream(wxString strExt, wxInputStream& stream,
                                   wxMBConv& conv /*=wxConvLocal*/)
  : wxArchiveInputStream(stream, conv),
  m_InStreamImpl(strExt, &stream),
  m_strExt(strExt)
{
    Init();
}

C7ZipInputStream::C7ZipInputStream(wxString strExt, wxInputStream *stream,
                                   wxMBConv& conv /*=wxConvLocal*/)
  : wxArchiveInputStream(stream, conv),
  m_InStreamImpl(strExt, stream),
  m_strExt(strExt)
{
    Init();
}

void C7ZipInputStream::Init()
{
	m_pEntryStream = NULL;

	m_nCurrentEntryIndex = 0;
	m_p7ZipArchive = NULL;
	
	m_7ZipLibrary.Initialize();

	if (m_7ZipLibrary.OpenArchive(&m_InStreamImpl, &m_p7ZipArchive))
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
					pEntry->SetArchiveIndex(pArchiveItem->GetArchiveIndex());

					m_Entries.push_back(pEntry);
				}
			}
		}
	}
	else
	{
		printf("open archive fail\n");
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

	WX_CLEAR_LIST(C7ZipEntryList_, m_Entries);

	CloseEntry();
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
	if (m_pEntryStream == NULL)
		return 0;

	return m_pEntryStream->Read(buffer, size).LastRead();
}

C7ZipEntry *C7ZipInputStream::GetNextEntry()
{
	if (m_nCurrentEntryIndex < m_Entries.size())
	{
		return m_Entries[m_nCurrentEntryIndex++]->Clone();
	}

	return NULL;
}

bool C7ZipInputStream::OpenEntry(wxArchiveEntry& entry)
{
    C7ZipEntry *zipEntry = wxStaticCast(&entry, C7ZipEntry);
    return zipEntry ? OpenEntry(*zipEntry) : false;
}

bool C7ZipInputStream::CloseEntry()
{
	if (m_pEntryStream != NULL)
	{
		delete m_pEntryStream;
	}

	m_pEntryStream = NULL;

	return true;
}

wxFileOffset C7ZipInputStream::OnSysSeek(wxFileOffset seek, wxSeekMode mode)
{
	if (m_pEntryStream == NULL)
		return -1;

	return m_pEntryStream->SeekI(seek, mode);
}

wxFileOffset C7ZipInputStream::OnSysTell() const
{
	if (m_pEntryStream == NULL)
		return -1;

	return m_pEntryStream->TellI();
}

wstring C7ZipInStreamImpl::GetExt() const
{
	return wstring(m_strExt.c_str());
}

int C7ZipInStreamImpl::Read(void *data, unsigned int size, unsigned int *processedSize)
{
	m_pInStream->Read(data, size);

	if (processedSize != NULL)
	{
		*processedSize = m_pInStream->LastRead();
	}

	return 0;
}

int C7ZipInStreamImpl::Seek(__int64 offset, unsigned int seekOrigin, unsigned __int64 *newPosition)
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

	if (m_pInStream->IsSeekable())
	{
		m_pInStream->SeekI(offset,mode);

		if (newPosition != NULL)
		{
			*newPosition = m_pInStream->TellI();
		}
	}

	return 0;
}

int C7ZipInStreamImpl::GetSize(unsigned __int64 * size)
{
	if (size != NULL)
	{
		*size = m_pInStream->GetLength();
	}

	return 0;
}

bool C7ZipInputStream::OpenEntry(C7ZipEntry & entry)
{
	wxString tmpfile =
		wxFileName::CreateTempFileName(wxT("wxreadbooktmp"));

	wxFileOutputStream sout(tmpfile);

	unsigned int nArchiveIndex = entry.GetArchiveIndex();

	C7ZipArchiveItem * pArchiveItem = NULL;

	if (!m_p7ZipArchive->GetItemInfo(nArchiveIndex, &pArchiveItem))
		return false;

	C7ZipOutStreamImpl outImpl(&sout);

	if (!m_p7ZipArchive->Extract(nArchiveIndex, &outImpl))
		return false;

	m_pEntryStream = new wxTemporaryFileInputStream(tmpfile);

	return true;
}

bool C7ZipInputStream::Eof() const
{
    if (m_pEntryStream == NULL)
        return true;

    return m_pEntryStream->Eof();
}
