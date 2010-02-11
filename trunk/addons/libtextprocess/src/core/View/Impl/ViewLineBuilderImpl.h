#pragma once

namespace TextProcess
{
	namespace View
	{
		namespace Impl
		{
			class CViewLineBuilderImpl :
				public virtual TextProcess::Impl::CLineBuilderImpl,
				public virtual IViewLineBuilder
			{
			public:
				CViewLineBuilderImpl(void);
				virtual ~CViewLineBuilderImpl(void);

			public:
				IMPL_PROPERTY(wxRect *, ClientArea);
				IMPL_PROPERTY(wxFont *, ViewFont);
				IMPL_PROPERTY(wxFileOffset, DocumentLineOffset);
				IMPL_PROPERTY(wxFileOffset, ViewLineOffset);
				IMPL_PROPERTY(TextProcess::Document::IDocumentLineManager *, DocumentLineManager);
				IMPL_PROPERTY(IViewLineManager *, ViewLineManager);
				IMPL_PROPERTY(wxDC *, Graphics);

			public:
				virtual int BuildLines();
				virtual void Cancel();

			private:
				IMPL_PROPERTY_FIELD(int, Cancel);
				void FixViewLineSize(TextProcess::Utils::wxReadOnlyString * pDocLineData,
                    wxFileOffset viewLineOffset, wxFileOffset & viewLineSize, long & curAllViewLineWidth);
			};
		}
	}
}
