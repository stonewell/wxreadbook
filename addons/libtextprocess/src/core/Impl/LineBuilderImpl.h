#pragma once

namespace TextProcess
{
	namespace Impl
	{
		class CLineBuilderImpl :
			public virtual ILineBuilder
		{
		public:
			CLineBuilderImpl(void);
			virtual ~CLineBuilderImpl(void);

		public:
			IMPL_PROPERTY(LineBuilderDirectionEnum, BuilderDirection);
			IMPL_PROPERTY(wxUint32, BuildLineCount);
			IMPL_PROPERTY(int, WaitForLineAccessed);
		};
	}
}
