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
			static IDocumentLine * CreateDocumentLine(int offset, int length);
			static IDocumentLineMatcher * CreateLineMatcher(int offset);
			static IDocumentLineManager * CreateLineManager();
			static IDocumentLineBuilder * CreateLineBuilder();
		};
	}
}
