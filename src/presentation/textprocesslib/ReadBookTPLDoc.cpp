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

#include "ReadBookTPLDoc.h"

class CDocumentBuilderRunnable: public PortableThread::IPortableRunnable {
public:
	unsigned long Run(void * pArgument) {
		TextProcess::Document::IDocumentLineBuilder
				* pLineBuilder =
						reinterpret_cast<TextProcess::Document::IDocumentLineBuilder *> (pArgument);

		return pLineBuilder->BuildLines();
	}
};
IMPLEMENT_DYNAMIC_CLASS(CReadBookTPLDoc, CReadBookDoc)

CReadBookTPLDoc::CReadBookTPLDoc(void) :
	m_BuildPrevThread(new CDocumentBuilderRunnable()), m_BuildNextThread(
			new CDocumentBuilderRunnable()), m_bDocumentLoading(false) {
}

CReadBookTPLDoc::~CReadBookTPLDoc(void) {
	StopDocumentLineBuilder();

	delete m_BuildPrevThread.GetPortableRunnable();
	delete m_BuildNextThread.GetPortableRunnable();
}

bool CReadBookTPLDoc::LoadBuffer(const wxString & url, wxMBConv * conv,
		bool bGuess) {
	StopDocumentLineBuilder();

	wxFileSystem fs;

	fs.ChangePathTo(url, false);

	std::auto_ptr<wxFSFile> pFile(fs.OpenFile(url, wxFS_READ | wxFS_SEEKABLE));

	if (pFile.get() == NULL)
		return false;

	wxInputStream * pInput = pFile->GetStream();

	wxMBConv * pConv = GetSuitableMBConv(pInput, conv, bGuess);

	if (IsArchiveFileUrl(url)) {
		m_pMemoryMappedFile.reset(
				TextProcess::IO::IMemoryMappedFile::CreateMemoryMappedFile(pInput,
						pConv));
	} else {
		m_pMemoryMappedFile.reset(
				TextProcess::IO::IMemoryMappedFile::CreateMemoryMappedFile(url, pConv));
	}

	TextProcess::Document::IDocumentLineManager * pDocumentLineManager =
			TextProcess::Document::CDocumentObjectFactory::CreateLineManager();
	m_pDocumentLineManager.reset(pDocumentLineManager);

	StartDocumentLineBuilder();
	return true;
}

wxFileOffset CReadBookTPLDoc::GetBufferSize(void) const {
	if (m_pMemoryMappedFile.get() == NULL)
		return 0;

	return m_pMemoryMappedFile->GetLength();
}

void CReadBookTPLDoc::StartDocumentLineBuilder() {
	CFileInfo * pFileInfo = NULL;

	wxGetApp().GetPreference()->GetFileInfo(GetFileName(), &pFileInfo);

	wxFileOffset docPos = 0;

	if (pFileInfo != NULL)
		docPos = pFileInfo->m_nCurrentLine;

	if (docPos < 0)
		docPos = 0;

	m_pDocumentLineBuilderPrev.reset(
			TextProcess::Document::CDocumentObjectFactory::CreateLineBuilder());

	m_pDocumentLineBuilderPrev->SetBuilderDirection(TextProcess::Prev);
	m_pDocumentLineBuilderPrev->SetDocumentFile(m_pMemoryMappedFile.get());
	m_pDocumentLineBuilderPrev->SetDocumentLineManager(
			m_pDocumentLineManager.get());
	m_pDocumentLineBuilderPrev->SetDocumentOffset(docPos);

	m_BuildPrevThread.SetRunningArgument(m_pDocumentLineBuilderPrev.get());
	m_BuildPrevThread.Start();

	m_pDocumentLineBuilderNext.reset(
			TextProcess::Document::CDocumentObjectFactory::CreateLineBuilder());

	m_pDocumentLineBuilderNext->SetBuilderDirection(TextProcess::Next);
	m_pDocumentLineBuilderNext->SetDocumentFile(m_pMemoryMappedFile.get());
	m_pDocumentLineBuilderNext->SetDocumentLineManager(
			m_pDocumentLineManager.get());
	m_pDocumentLineBuilderNext->SetDocumentOffset(docPos);

	m_BuildNextThread.SetRunningArgument(m_pDocumentLineBuilderNext.get());
	m_BuildNextThread.Start();

	SetCurrentLine(docPos);

	m_bDocumentLoading = true;
}

void CReadBookTPLDoc::StopDocumentLineBuilder() {
	m_bDocumentLoading = false;

	if (m_pDocumentLineBuilderPrev.get() != NULL) {
		m_pDocumentLineBuilderPrev->Cancel();
		if (!m_BuildPrevThread.Join(3000))
			m_BuildPrevThread.Abort();
	}

	if (m_pDocumentLineBuilderNext.get() != NULL) {
		m_pDocumentLineBuilderNext->Cancel();
		if (!m_BuildNextThread.Join(3000))
			m_BuildNextThread.Abort();
	}
}

void CReadBookTPLDoc::ScrollDocumentTo(wxFileOffset nOffset) {
	SetCurrentLine(nOffset);

	StopDocumentLineBuilder();

	m_pDocumentLineManager.reset(
			TextProcess::Document::CDocumentObjectFactory::CreateLineManager());
	m_pMemoryMappedFile->Reset();
	StartDocumentLineBuilder();
}

bool CReadBookTPLDoc::GetDocumentLineInfo(wxInt32 nPos, wxInt32 & nOffset,
		wxInt32 & nLength, wxInt32 & nDecodedLength) {
	if (!m_bDocumentLoading)
		return false;
	std::auto_ptr<TextProcess::Document::IDocumentLineMatcher> pMatcher(
			TextProcess::Document::CDocumentObjectFactory::CreateLineMatcher(nPos));

	TextProcess::Document::IDocumentLine * pLine =
			m_pDocumentLineManager->FindLine(pMatcher.get(), 0);

	if (pLine == NULL) {
		std::auto_ptr<TextProcess::Document::IDocumentLineManager>
				pDocumentLineManager(
						TextProcess::Document::CDocumentObjectFactory::CreateLineManager());

		std::auto_ptr<TextProcess::Document::IDocumentLineBuilder>
				pDocumentLineBuilder(
						TextProcess::Document::CDocumentObjectFactory::CreateLineBuilder());

		pDocumentLineBuilder->SetBuilderDirection(TextProcess::Next);
		pDocumentLineBuilder->SetDocumentFile(m_pMemoryMappedFile.get());
		pDocumentLineBuilder->SetDocumentLineManager(pDocumentLineManager.get());
		pDocumentLineBuilder->SetDocumentOffset(nPos);
		pDocumentLineBuilder->SetBuildLineCount(1);
		pDocumentLineBuilder->SetWaitForLineAccessed(0);
		pDocumentLineBuilder->BuildLines();

		pLine = pDocumentLineManager->FindLine(pMatcher.get(), 0);

		if (pLine == NULL)
			return false;

		nOffset = pLine->GetOffset();
		nLength = pLine->GetLength();
		nDecodedLength = pLine->GetDecodedLength();
	} else {
		nOffset = pLine->GetOffset();
		nLength = pLine->GetLength();
		nDecodedLength = pLine->GetDecodedLength();
	}

	return true;
}
