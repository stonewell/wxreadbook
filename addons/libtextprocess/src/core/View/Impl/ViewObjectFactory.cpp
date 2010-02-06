#include "../../TextProcess.h"
#include "../../Impl/TextProcessImpl.h"

TextProcess::View::IViewLine * TextProcess::View::CViewObjectFactory::CreateViewLine(int viewLineOffset, int viewLineSize,
    TextProcess::Document::IDocumentLine * pDocumentLine)
{
	TextProcess::View::IViewLine * pViewLine = new TextProcess::View::Impl::CViewLineImpl();

	pViewLine->SetOffset(viewLineOffset);
	pViewLine->SetLength(viewLineSize);
	pViewLine->SetDocumentLine(pDocumentLine);

	return pViewLine;
}

TextProcess::View::IViewLineMatcher * TextProcess::View::CViewObjectFactory::CreateLineMatcher(int docOffset, int viewOffset)
{
	TextProcess::View::IViewLineMatcher * pMatcher =
		new TextProcess::View::Impl::CViewLineMatcherImpl();

	pMatcher->SetDocumentLineOffset(docOffset);
	pMatcher->SetViewLineOffset(viewOffset);

	return pMatcher;
}

TextProcess::View::IViewLineManager * TextProcess::View::CViewObjectFactory::CreateLineManager()
{
	return new TextProcess::View::Impl::CViewLineManagerImpl();
}

TextProcess::View::IViewLineBuilder * TextProcess::View::CViewObjectFactory::CreateLineBuilder()
{
	return new TextProcess::View::Impl::CViewLineBuilderImpl();
}
