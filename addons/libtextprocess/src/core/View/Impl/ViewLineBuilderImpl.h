#pragma once

#include <vector>

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
				IMPL_PROPERTY(IViewLineBuilderGraphic *, Graphics);

			public:
				virtual int BuildLines();
				virtual void Cancel();

			private:
				IMPL_PROPERTY_FIELD(int, Cancel);
				void FixViewLineSize(TextProcess::Utils::Impl::wxReadOnlyString * pDocLineData,
					wxFileOffset viewLineOffset, wxFileOffset & viewLineSize, long & curAllViewLineWidth);
				void FixViewLineSize(TextProcess::Utils::Impl::wxReadOnlyString * pDocLineData,
					wxFileOffset offset, wxFileOffset & size);
				int InternalBuildLines(TextProcess::Document::IDocumentLine * pDocLine,
					wxFileOffset startOffset,
					wxFileOffset & endOffset,
					wxInt32 nBuildCount,
					std::vector<TextProcess::View::IViewLine *> & lines);
				wxInt32 CalculateDefaultLineCharSize(TextProcess::Document::IDocumentLine * pDocLine);
			};
		}
	}
}
