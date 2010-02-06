#pragma once

namespace TextProcess
{
	namespace Document
	{
		class IDocumentLineBuilder :
			public virtual ILineBuilder
		{
		public:
			DECLARE_PROPERTY(int, DocumentOffset);
			DECLARE_PROPERTY(IDocumentLineManager *, DocumentLineManager);
			DECLARE_PROPERTY(TextProcess::IO::IMemoryMappedFile *, DocumentFile);
		};
	}
}
