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

				IMPL_PROPERTY_FIELD(wxCharBuffer, CRBuffer);
				IMPL_PROPERTY_FIELD(wxCharBuffer, LFBuffer);
				IMPL_PROPERTY_FIELD(wxCharBuffer, SpaceBuffer);
				IMPL_PROPERTY_FIELD(wxCharBuffer, TabBuffer);
				IMPL_PROPERTY_FIELD(int, CRLength);
				IMPL_PROPERTY_FIELD(int, LFLength);
				IMPL_PROPERTY_FIELD(int, SpaceLength);
				IMPL_PROPERTY_FIELD(int, TabLength);

				void InitBuffers();
				int IsEmptyLine(int offset, int length);
			};
		}
	}
}
