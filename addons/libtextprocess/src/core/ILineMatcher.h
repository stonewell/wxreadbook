#pragma once

namespace TextProcess
{
	class ILineMatcher
	{
	public:
		virtual int IsMatch(const ILine * pLine) const = 0;
	};

}
