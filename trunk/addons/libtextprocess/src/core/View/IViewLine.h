#pragma once

namespace TextProcess
{
	namespace View
	{
		class IViewLine : public virtual ILine
		{
		public:
			DECLARE_TPL_INTERFACE(IViewLine);
			DECLARE_PROPERTY(TextProcess::Document::IDocumentLine *, DocumentLine);
		};
	}

}
