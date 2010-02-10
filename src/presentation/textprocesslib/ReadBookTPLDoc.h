#pragma once

#include "../ReadBookDoc.h"
#include <memory>
#include "TextProcess.h"
#include "PortableThread.h"

class CReadBookTPLDoc :
	public CReadBookDoc
{
    DECLARE_DYNAMIC_CLASS(CReadBookTPLDoc)

public:
	CReadBookTPLDoc(void);
	virtual ~CReadBookTPLDoc(void);

public:
	virtual wxFileOffset GetBufferSize(void) const;

	TextProcess::Document::IDocumentLineManager * GetDocumentLineManager() const { return m_pDocumentLineManager.get(); }
	bool IsDocumentLoading() const { return m_bDocumentLoading; }

protected:
	virtual bool LoadBuffer(const wxString & url, wxMBConv * conv, bool bGuess);

	virtual void StartDocumentLineBuilder();
	virtual void StopDocumentLineBuilder();
private:
	std::auto_ptr<TextProcess::Document::IDocumentLineManager> m_pDocumentLineManager;
	std::auto_ptr<TextProcess::IO::IMemoryMappedFile> m_pMemoryMappedFile;
	std::auto_ptr<TextProcess::Document::IDocumentLineBuilder> m_pDocumentLineBuilderPrev;
	std::auto_ptr<TextProcess::Document::IDocumentLineBuilder> m_pDocumentLineBuilderNext;
	PortableThread::CPortableThread m_BuildPrevThread;
	PortableThread::CPortableThread m_BuildNextThread;
	bool m_bDocumentLoading;
};
