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

			IMPL_PROPERTY(int, Offset);
			IMPL_PROPERTY(int, Length);
			IMPL_PROPERTY(int, Index);
		};
	}
}
