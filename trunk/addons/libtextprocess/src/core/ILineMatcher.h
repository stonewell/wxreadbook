#pragma once

namespace TextProcess
{
	class ILineMatcher
	{
	public:
		DECLARE_TPL_INTERFACE(ILineMatcher);

		virtual int IsMatch(const ILine * pLine) const = 0;
		virtual int IsBeforeLine(const ILine * pLine) const = 0;
		virtual int IsAfterLine(const ILine * pLine) const = 0;
		virtual int Compare(const ILine * pLine) const = 0;
	};

}
