// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filesys.h"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "../../ReadBookApp.h"
#include "../../ReadBookMainFrm.h"
#include "ReadBookTPLDoc.h"

IMPLEMENT_DYNAMIC_CLASS(CReadBookTPLDoc, CReadBookDoc)

CReadBookTPLDoc::CReadBookTPLDoc(void)
{
}

CReadBookTPLDoc::~CReadBookTPLDoc(void)
{
}

bool CReadBookTPLDoc::LoadBuffer(const wxString & url, wxMBConv * conv, bool bGuess)
{
	StopDocumentLineBuilder();

	wxFileSystem fs;

	fs.ChangePathTo(url, false);

	std::auto_ptr<wxFSFile> pFile(fs.OpenFile(url, wxFS_READ | wxFS_SEEKABLE));

	if (pFile.get() == NULL)
		return false;

	wxInputStream * pInput = pFile->GetStream();

	wxMBConv * pConv = GetSuitableMBConv(pInput, conv, bGuess);

	if (IsArchiveFileUrl(url))
	{
		std::auto_ptr<TextProcess::IO::IMemoryMappedFile> file(TextProcess::IO::IMemoryMappedFile::CreateMemoryMappedFile(pInput, pConv));

		m_pMemoryMappedFile = file;
	}
	else
	{
		std::auto_ptr<TextProcess::IO::IMemoryMappedFile> file(TextProcess::IO::IMemoryMappedFile::CreateMemoryMappedFile(url, pConv));

		m_pMemoryMappedFile = file;
	}

	std::auto_ptr<TextProcess::Document::IDocumentLineManager> docLineManager(TextProcess::Document::CDocumentObjectFactory::CreateLineManager());

	m_pDocumentLineManager = docLineManager;

	StartDocumentLineBuilder();

	return true;
}

wxFileOffset CReadBookTPLDoc::GetBufferSize(void) const
{
	return m_pMemoryMappedFile->GetLength();
}

void CReadBookTPLDoc::StartDocumentLineBuilder()
{
	CFileInfo * pFileInfo = NULL;

	wxGetApp().GetPreference()->GetFileInfo(GetFileName(), &pFileInfo);

	wxFileOffset docPos = 0;

	if (pFileInfo != NULL)
		docPos = pFileInfo->m_nFilePos;

	if (docPos > 0)
	{
	}
	
	if (docPos < GetBufferSize())
	{
	}
}

void CReadBookTPLDoc::StopDocumentLineBuilder()
{
}
