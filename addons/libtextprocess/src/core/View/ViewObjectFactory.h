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
			static IViewLine * CreateViewLine(wxFileOffset viewLineOffset, wxFileOffset viewLineSize, TextProcess::Document::IDocumentLine * pDocumentLine);
			static IViewLineMatcher * CreateLineMatcher(wxFileOffset docOffset, wxFileOffset viewOffset);
			static IViewLineManager * CreateLineManager();
			static IViewLineBuilder * CreateLineBuilder();
		};
	}
}
