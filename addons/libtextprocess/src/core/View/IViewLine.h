#pragma once

namespace TextProcess
{
	namespace View
	{
		class IViewLine : public virtual ILine
		{
		public:
			DECLARE_PROPERTY(TextProcess::Document::IDocumentLine *, DocumentLine);
		};
	}

}
