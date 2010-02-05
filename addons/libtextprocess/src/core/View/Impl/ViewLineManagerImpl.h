#pragma once

namespace TextProcess
{
	namespace View
	{
		namespace Impl
		{
			class CViewLineManagerImpl :
				public IViewLineManager
				,public TextProcess::Impl::CLineManagerImpl<IViewLine, IViewLineMatcher>
			{
			public:
				CViewLineManagerImpl(void);
				virtual ~CViewLineManagerImpl(void);

			public:
				virtual IViewLine * FindLine(IViewLineMatcher * pMatcher, int wait = 1)
				{
					return TextProcess::Impl::CLineManagerImpl<IViewLine, IViewLineMatcher>::FindLine(pMatcher, wait);
				}

				virtual IViewLine * GetNextLine(IViewLine * pLine, int wait = 1)
				{
					return TextProcess::Impl::CLineManagerImpl<IViewLine, IViewLineMatcher>::GetNextLine(pLine, wait);
				}

				virtual IViewLine * GetPrevLine(IViewLine * pLine, int wait = 1)
				{
					return TextProcess::Impl::CLineManagerImpl<IViewLine, IViewLineMatcher>::GetPrevLine(pLine, wait);
				}

				virtual void AddNextLine(IViewLine * pNextLine, IViewLine * pLine)
				{
					TextProcess::Impl::CLineManagerImpl<IViewLine, IViewLineMatcher>::AddNextLine(pNextLine, pLine);
				}

				virtual void AddPrevLine(IViewLine * pPrevLine, IViewLine * pLine)
				{
					TextProcess::Impl::CLineManagerImpl<IViewLine, IViewLineMatcher>::AddPrevLine(pPrevLine, pLine);
				}

				virtual void Clear()
				{
					TextProcess::Impl::CLineManagerImpl<IViewLine, IViewLineMatcher>::Clear();
				}

				virtual void HasAllLines(int getAllLines = 1)
				{
					TextProcess::Impl::CLineManagerImpl<IViewLine, IViewLineMatcher>::HasAllLines(getAllLines);
				}

				virtual int IsHasAllLines()
				{
					return TextProcess::Impl::CLineManagerImpl<IViewLine, IViewLineMatcher>::IsHasAllLines();
				}
			};
		}
	}
}
