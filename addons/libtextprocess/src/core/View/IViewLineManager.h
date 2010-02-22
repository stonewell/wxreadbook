#pragma once

namespace TextProcess
{
	namespace View
	{
		class IViewLineManager
		{
		public:
			DECLARE_TPL_INTERFACE(IViewLineManager);

			virtual IViewLine * FindLine(IViewLineMatcher * pMatcher, int wait = 1) = 0;

			virtual IViewLine * GetNextLine(IViewLine * pLine, int wait = 1) = 0;

			virtual IViewLine * GetPrevLine(IViewLine * pLine, int wait = 1) = 0;
			virtual IViewLine * GetHeaderLine(int wait = 1) = 0;
			virtual IViewLine * GetTailLine(int wait = 1) = 0;

			virtual void AddNextLine(IViewLine * pNextLine, IViewLine * pLine) = 0;
			virtual void AddPrevLine(IViewLine * pPrevLine, IViewLine * pLine) = 0;

			virtual void Clear() = 0;

			virtual void HasAllPreviousLines() = 0;
			virtual void HasAllNextLines() = 0;

			virtual int IsHasAllLines() = 0;
			virtual int IsHasAllPreviousLines() = 0;
			virtual int IsHasAllNextLines() = 0;
		};
	}
}
