#pragma once

#include <vector>

namespace TextProcess
{
	namespace View
	{
		namespace Impl
		{
			class CViewLineBuilderImpl2 :
				public virtual TextProcess::Impl::CLineBuilderImpl,
				public virtual IViewLineBuilder
			{
			public:
				CViewLineBuilderImpl2(void);
				virtual ~CViewLineBuilderImpl2(void);

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
				void FixViewLineSize(const wxChar * pBuf,
					size_t pBufLen,
					wxFileOffset viewLineOffset, wxFileOffset & viewLineSize, 
					int unicodeCharWidth,
					int asciiCharWidth);
				int InternalBuildLines(TextProcess::Document::IDocumentLine * pDocLine,
					wxFileOffset startOffset,
					wxFileOffset & endOffset,
					wxInt32 nBuildCount,
					std::vector<TextProcess::View::IViewLine *> & lines,
					int defaultLineCharSize,
					int unicodeCharWidth,
					int asciiCharWidth);
				wxInt32 CalculateDefaultLineCharSize(int & unicodeCharWidth,
					int & asciiCharWidth);
			};
		}
	}
}
