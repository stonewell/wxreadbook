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

C7ZipOutputStream::C7ZipOutputStream(wxOutputStream& stream,
								   int level      /*=-1*/,
								   wxMBConv& conv /*=wxConvLocal*/)
								   : wxArchiveOutputStream(stream, conv)
{
	Init(level);
}

C7ZipOutputStream::C7ZipOutputStream(wxOutputStream *stream,
								   int level      /*=-1*/,
								   wxMBConv& conv /*=wxConvLocal*/)
								   : wxArchiveOutputStream(stream, conv)
{
	Init(level);
}

void C7ZipOutputStream::Init(int level)
{
	m_initialData = new char[1024];
	m_initialSize = 0;
	m_pending = NULL;
	m_raw = false;
	m_headerOffset = 0;
	m_headerSize = 0;
	m_entrySize = 0;
	m_comp = NULL;
	m_level = level;
	m_offsetAdjustment = wxInvalidOffset;
}

C7ZipOutputStream::~C7ZipOutputStream()
{
	Close();
	WX_CLEAR_LIST(C7ZipEntryList_, m_entries);
	delete [] m_initialData;
}

bool C7ZipOutputStream::CloseCompressor(wxOutputStream *comp)
{
	return false;
}

wxOutputStream *C7ZipOutputStream::OpenCompressor(
	wxOutputStream& stream,
	C7ZipEntry& entry,
	const Buffer bufs[])
{
	return NULL;
}

bool C7ZipOutputStream::PutNextEntry(wxArchiveEntry *entry)
{
	return false;
}

bool C7ZipOutputStream::CopyEntry(wxArchiveEntry *entry, wxArchiveInputStream& stream)
{
	return false;
}

bool C7ZipOutputStream::CopyArchiveMetaData(wxArchiveInputStream& stream)
{
	return false;
}

bool C7ZipOutputStream::PutNextEntry(const wxString& name,
									const wxDateTime& dt,
									wxFileOffset size)
{
	return false;
}

bool C7ZipOutputStream::PutNextDirEntry(const wxString& name,
									   const wxDateTime& dt)
{
	return false;
}

bool C7ZipOutputStream::CopyEntry(C7ZipEntry *entry, C7ZipInputStream& inputStream)
{
	return false;
}

bool C7ZipOutputStream::CopyArchiveMetaData(C7ZipInputStream& inputStream)
{
	return false;
}

void C7ZipOutputStream::Sync()
{
}

bool C7ZipOutputStream::CloseEntry()
{
	return false;
}

bool C7ZipOutputStream::Close()
{
	return false;
}

void C7ZipOutputStream::SetLevel(int level)
{
}

size_t C7ZipOutputStream::OnSysWrite(const void *buffer, size_t size)
{
	return 0;
}
