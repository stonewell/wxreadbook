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
}

C7ZipInputStream::~C7ZipInputStream()
{
	if (m_p7ZipArchive != NULL)
	{
		m_p7ZipArchive->Close();
		delete m_p7ZipArchive;
	}

	m_7ZipLibrary.Deinitialize();
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
	return 0;
}

C7ZipEntry *C7ZipInputStream::GetNextEntry()
{
	C7ZipEntry * pEntries[] = {
		new C7ZipEntry(wxT("a.txt")),
		new C7ZipEntry(wxT("b.txt")),
		new C7ZipEntry(wxT("c\\1.txt")),
		new C7ZipEntry(wxT("c\\2.txt")),
		new C7ZipEntry(wxT("c")),
	};

	if (m_nCurrentEntryIndex < sizeof(pEntries) / sizeof(C7ZipEntry*))
	{
		if (m_nCurrentEntryIndex == sizeof(pEntries) / sizeof(C7ZipEntry*) - 1)
		{
			pEntries[m_nCurrentEntryIndex]->SetIsDir(true);
		}

		return pEntries[m_nCurrentEntryIndex++];
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

wxFileOffset C7ZipInputStream::GetLength() const
{
	return 0;
}