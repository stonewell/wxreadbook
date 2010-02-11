#pragma once

namespace TextProcess
{
	class ILineMatcher
	{
	public:
		DECLARE_TPL_INTERFACE(ILineMatcher);

		virtual int IsMatch(const ILine * pLine) const = 0;
	};

}
