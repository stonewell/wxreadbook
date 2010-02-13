#pragma once


namespace TextProcess
{
	namespace Document
	{
		namespace Impl
		{
			class CDocumentLineMatcherImpl :
				public virtual IDocumentLineMatcher
			{
			public:
				CDocumentLineMatcherImpl(void);
				virtual ~CDocumentLineMatcherImpl(void);

			public:
				IMPL_PROPERTY(wxFileOffset, DocumentOffset);

				virtual int IsMatch(const ILine * pLine) const;
				virtual int IsBeforeLine(const ILine * pLine) const;
				virtual int IsAfterLine(const ILine * pLine) const;
			};
		}
	}
}
