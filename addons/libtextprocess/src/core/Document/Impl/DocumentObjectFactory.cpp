#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::Document::IDocumentLine * TextProcess::Document::CDocumentObjectFactory::CreateDocumentLine(wxFileOffset offset, wxFileOffset length, TextProcess::IO::IMemoryMappedFile * pDocumentFile)
{
	TextProcess::Document::IDocumentLine * pDocumentLine = new TextProcess::Document::Impl::CDocumentLineImpl();

	pDocumentLine->SetOffset(offset);
	pDocumentLine->SetLength(length);
	pDocumentLine->SetDocumentFile(pDocumentFile);

	return pDocumentLine;
}

TextProcess::Document::IDocumentLineMatcher * TextProcess::Document::CDocumentObjectFactory::CreateLineMatcher(wxFileOffset docOffset)
{
	TextProcess::Document::IDocumentLineMatcher * pMatcher =
		new TextProcess::Document::Impl::CDocumentLineMatcherImpl();

	pMatcher->SetDocumentOffset(docOffset);

	return pMatcher;
}

TextProcess::Document::IDocumentLineManager * TextProcess::Document::CDocumentObjectFactory::CreateLineManager()
{
	return new TextProcess::Document::Impl::CDocumentLineManagerImpl();
}

TextProcess::Document::IDocumentLineBuilder * TextProcess::Document::CDocumentObjectFactory::CreateLineBuilder()
{
	return new TextProcess::Document::Impl::CDocumentLineBuilderImpl();
}
