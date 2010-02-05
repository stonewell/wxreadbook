#pragma once

namespace TextProcess
{
	namespace View
	{
		class IViewLineMatcher : public virtual ILineMatcher
		{
		public:
			DECLARE_PROPERTY(int, DocumentLineOffset);
			DECLARE_PROPERTY(int, ViewLineOffset);
		};
	}

}
