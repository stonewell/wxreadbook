#pragma once

namespace TextProcess
{
	namespace Document
	{
		class IDocumentLineMatcher :
			public virtual ILineMatcher
		{
		public:
			DECLARE_PROPERTY(int, DocumentOffset);
		};
	}
}
