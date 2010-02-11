#pragma once

namespace TextProcess
{
	namespace View
	{
		class IViewLineMatcher : public virtual ILineMatcher
		{
		public:
			DECLARE_TPL_INTERFACE(IViewLineMatcher);
			DECLARE_PROPERTY(wxFileOffset, DocumentLineOffset);
			DECLARE_PROPERTY(wxFileOffset, ViewLineOffset);
		};
	}

}
