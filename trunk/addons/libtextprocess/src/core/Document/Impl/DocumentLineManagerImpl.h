#pragma once

namespace TextProcess
{
	namespace Document
	{
		namespace Impl
		{
			class CDocumentLineManagerImpl :
				public IDocumentLineManager
				,public TextProcess::Impl::CLineManagerImpl<IDocumentLine, IDocumentLineMatcher>
			{
			public:
				CDocumentLineManagerImpl(void);
				virtual ~CDocumentLineManagerImpl(void);

			public:
				virtual IDocumentLine * FindLine(IDocumentLineMatcher * pMatcher, int wait = 1)
				{
					return TextProcess::Impl::CLineManagerImpl<IDocumentLine, IDocumentLineMatcher>::FindLine(pMatcher, wait);
				}

				virtual IDocumentLine * GetNextLine(IDocumentLine * pLine, int wait = 1)
				{
					return TextProcess::Impl::CLineManagerImpl<IDocumentLine, IDocumentLineMatcher>::GetNextLine(pLine, wait);
				}

				virtual IDocumentLine * GetPrevLine(IDocumentLine * pLine, int wait = 1)
				{
					return TextProcess::Impl::CLineManagerImpl<IDocumentLine, IDocumentLineMatcher>::GetPrevLine(pLine, wait);
				}

				virtual void AddNextLine(IDocumentLine * pNextLine, IDocumentLine * pLine)
				{
					TextProcess::Impl::CLineManagerImpl<IDocumentLine, IDocumentLineMatcher>::AddNextLine(pNextLine, pLine);
				}

				virtual void AddPrevLine(IDocumentLine * pPrevLine, IDocumentLine * pLine)
				{
					TextProcess::Impl::CLineManagerImpl<IDocumentLine, IDocumentLineMatcher>::AddPrevLine(pPrevLine, pLine);
				}

				virtual void Clear()
				{
					TextProcess::Impl::CLineManagerImpl<IDocumentLine, IDocumentLineMatcher>::Clear();
				}

				virtual void HasAllPreviousLines()
				{
					TextProcess::Impl::CLineManagerImpl<IDocumentLine, IDocumentLineMatcher>::HasAllPreviousLines();
				}

				virtual void HasAllNextLines()
				{
					TextProcess::Impl::CLineManagerImpl<IDocumentLine, IDocumentLineMatcher>::HasAllNextLines();
				}

				virtual int IsHasAllLines()
				{
					return TextProcess::Impl::CLineManagerImpl<IDocumentLine, IDocumentLineMatcher>::IsHasAllLines();
				}
			};
		}
	}
}
