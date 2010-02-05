#pragma once

namespace TextProcess
{
	namespace View
	{
		namespace Impl
		{
			class CViewLineImpl :
				virtual public IViewLine,
				virtual public TextProcess::Impl::CLineImpl
			{
			public:
				CViewLineImpl(void);
				virtual ~CViewLineImpl(void);

			public:
				IMPL_PROPERTY(TextProcess::Document::IDocumentLine *, DocumentLine);
			};
		}
	}
}
