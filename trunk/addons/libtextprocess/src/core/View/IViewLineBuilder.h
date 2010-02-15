#pragma once

namespace TextProcess
{
	namespace View
	{
		class IViewLineBuilderGraphic
		{
		public:
			DECLARE_TPL_INTERFACE(IViewLineBuilderGraphic);
    		virtual void GetTextExtent(const wxString& string,
                       wxCoord *x, wxCoord *y,
                       wxFont *theFont = NULL) const = 0;
		};

		class IViewLineBuilder :
			public virtual ILineBuilder
		{
		public:
			DECLARE_TPL_INTERFACE(IViewLineBuilder);
			DECLARE_PROPERTY(wxRect *, ClientArea);
			DECLARE_PROPERTY(wxFont *, ViewFont);
			DECLARE_PROPERTY(IViewLineBuilderGraphic *, Graphics);
			DECLARE_PROPERTY(wxFileOffset, DocumentLineOffset);
			DECLARE_PROPERTY(wxFileOffset, ViewLineOffset);
			DECLARE_PROPERTY(TextProcess::Document::IDocumentLineManager *, DocumentLineManager);
			DECLARE_PROPERTY(IViewLineManager *, ViewLineManager);
		};
	}
}
