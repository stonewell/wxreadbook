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
	void ScrollDocumentTo(wxFileOffset nOffset);
	virtual bool GetDocumentLineInfo(wxInt32 nPos, 
		wxInt32 & nOffset, wxInt32 & nLength, wxInt32 & nDecodedLength);

protected:
	virtual bool LoadBuffer(const wxString & url, wxMBConv * conv, bool bGuess);

	virtual void StartDocumentLineBuilder();
	virtual void StopDocumentLineBuilder();
private:
	std::shared_ptr<TextProcess::Document::IDocumentLineManager> m_pDocumentLineManager;
	std::shared_ptr<TextProcess::IO::IMemoryMappedFile> m_pMemoryMappedFile;
	std::shared_ptr<TextProcess::Document::IDocumentLineBuilder> m_pDocumentLineBuilderPrev;
	std::shared_ptr<TextProcess::Document::IDocumentLineBuilder> m_pDocumentLineBuilderNext;
	PortableThread::CPortableThread m_BuildPrevThread;
	PortableThread::CPortableThread m_BuildNextThread;
	bool m_bDocumentLoading;
};
