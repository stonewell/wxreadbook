#pragma once

namespace TextProcess
{
	namespace View
	{
		namespace Impl
		{
			class CViewLineMatcherImpl :
				public virtual IViewLineMatcher
			{
			public:
				CViewLineMatcherImpl(void);
				virtual ~CViewLineMatcherImpl(void);

			public:
				IMPL_PROPERTY(int, DocumentLineOffset);
				IMPL_PROPERTY(int, ViewLineOffset);

				virtual int IsMatch(const ILine * pLine) const;
			};
		}
	}
}
