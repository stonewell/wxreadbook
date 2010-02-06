#pragma once

namespace TextProcess
{
	namespace View
	{
		class CViewObjectFactory
		{
		private:
			CViewObjectFactory() {}

		public:
			static IViewLine * CreateViewLine(int viewLineOffset, int viewLineSize, TextProcess::Document::IDocumentLine * pDocumentLine);
			static IViewLineMatcher * CreateLineMatcher(int docOffset, int viewOffset);
			static IViewLineManager * CreateLineManager();
			static IViewLineBuilder * CreateLineBuilder();
		};
	}
}
