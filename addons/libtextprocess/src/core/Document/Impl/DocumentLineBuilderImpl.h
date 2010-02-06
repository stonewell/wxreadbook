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
				IMPL_PROPERTY(TextProcess::IO::IMemoryMappedFile *, DocumentFile);

			public:
				virtual int BuildLines();
				virtual void Cancel();

			private:
				IMPL_PROPERTY_FIELD(int, Cancel);

				int IsEmptyLine(int offset, int length);
			};
		}
	}
}
