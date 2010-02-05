#pragma once

namespace TextProcess
{
	namespace Document
	{
		namespace Impl
		{
			class CDocumentLineBuilderImpl :
				public virtual TextProcess::Impl::CLineBuilderImpl,
				public virtual IDocumentLineBuilder
			{
			public:
				CDocumentLineBuilderImpl(void);
				virtual ~CDocumentLineBuilderImpl(void);

			public:
				IMPL_PROPERTY(int, DocumentOffset);
				IMPL_PROPERTY(IDocumentLineManager *, DocumentLineManager);

			public:
				virtual int BuildLines();
				virtual void Cancel();
			};
		}
	}
}
