#pragma once

#include "../ReadBookDoc.h"
#include <memory>
#include "../../../addons/libtextprocess/src/core/TextProcess.h"

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

protected:
	virtual bool LoadBuffer(const wxString & url, wxMBConv * conv, bool bGuess);

	virtual void StartDocumentLineBuilder();
	virtual void StopDocumentLineBuilder();
private:
	std::auto_ptr<TextProcess::Document::IDocumentLineManager> m_pDocumentLineManager;
	std::auto_ptr<TextProcess::IO::IMemoryMappedFile> m_pMemoryMappedFile;
};
