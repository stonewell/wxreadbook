#pragma once

namespace TextProcess
{
	namespace View
	{
		class IViewLineBuilder :
			public virtual ILineBuilder
		{
		public:
			DECLARE_PROPERTY(wxRect *, ClientArea);
			DECLARE_PROPERTY(wxFont *, ViewFont);
			DECLARE_PROPERTY(wxDC *, Graphics);
			DECLARE_PROPERTY(int, DocumentLineOffset);
			DECLARE_PROPERTY(int, ViewLineOffset);
			DECLARE_PROPERTY(TextProcess::Document::IDocumentLineManager *, DocumentLineManager);
			DECLARE_PROPERTY(IViewLineManager *, ViewLineManager);
		};
	}
}
