#pragma once

namespace TextProcess
{
	namespace Document
	{
		class IDocumentLineBuilder :
			public virtual ILineBuilder
		{
		public:
			DECLARE_TPL_INTERFACE(IDocumentLineBuilder);
			DECLARE_PROPERTY(wxFileOffset, DocumentOffset);
			DECLARE_PROPERTY(IDocumentLineManager *, DocumentLineManager);
			DECLARE_PROPERTY(TextProcess::IO::IMemoryMappedFile *, DocumentFile);
		};
	}
}
