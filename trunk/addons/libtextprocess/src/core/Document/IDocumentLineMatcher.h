#pragma once

namespace TextProcess
{
	namespace Document
	{
		class IDocumentLineMatcher :
			public virtual ILineMatcher
		{
		public:
			DECLARE_TPL_INTERFACE(IDocumentLineMatcher);
			DECLARE_PROPERTY(wxFileOffset, DocumentOffset);
		};
	}
}
