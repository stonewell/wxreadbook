#pragma once

namespace TextProcess
{
	namespace Impl
	{
		class CLineImpl : public virtual ILine
		{
		public:
			CLineImpl(LineTypeEnum nLineType);
			virtual ~CLineImpl(void);

		public:
			IMPL_PROPERTY(LineTypeEnum, Type);

			IMPL_PROPERTY(wxFileOffset, Offset);
			IMPL_PROPERTY(wxFileOffset, Length);
			IMPL_PROPERTY(wxUint32, Index);
		};
	}
}
