#pragma once

namespace TextProcess
{
	namespace Document
	{
		class CDocumentObjectFactory
		{
		private:
			CDocumentObjectFactory() {}

		public:
			static IDocumentLine * CreateDocumentLine(wxFileOffset offset, wxFileOffset length, TextProcess::IO::IMemoryMappedFile * pDocumentFile);
			static IDocumentLineMatcher * CreateLineMatcher(wxFileOffset offset);
			static IDocumentLineManager * CreateLineManager();
			static IDocumentLineBuilder * CreateLineBuilder();
		};
	}
}
