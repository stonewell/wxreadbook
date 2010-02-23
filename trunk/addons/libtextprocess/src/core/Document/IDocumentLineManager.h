#pragma once

namespace TextProcess
{
	namespace Document
	{
		class IDocumentLineManager
		{
		public:
			DECLARE_TPL_INTERFACE(IDocumentLineManager);

			virtual IDocumentLine * FindLine(IDocumentLineMatcher * pMatcher, int wait = 1) = 0;

			virtual IDocumentLine * GetNextLine(IDocumentLine * pLine, int wait = 1) = 0;

			virtual IDocumentLine * GetPrevLine(IDocumentLine * pLine, int wait = 1) = 0;
			virtual IDocumentLine * GetHeaderLine(int wait = 1) = 0;
			virtual IDocumentLine * GetTailLine(int wait = 1) = 0;

			virtual void AddNextLine(IDocumentLine * pNextLine, IDocumentLine * pLine) = 0;
			virtual void AddPrevLine(IDocumentLine * pPrevLine, IDocumentLine * pLine) = 0;

			virtual void Clear() = 0;

			virtual void HasAllPreviousLines() = 0;
			virtual void HasAllNextLines() = 0;

			virtual int IsHasAllLines() = 0;
		};
	}
}
