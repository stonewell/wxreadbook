#pragma once

namespace TextProcess
{
	class ILine
	{
	public:
		enum LineTypeEnum
		{
			DocumentLine,
			ViewLine
		};

	public:
		DECLARE_PROPERTY_GETTER(LineTypeEnum, Type);

		DECLARE_PROPERTY(int, Offset);
		DECLARE_PROPERTY(int, Length);
		DECLARE_PROPERTY(int, Index);
	};
}
