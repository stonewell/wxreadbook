#pragma once

namespace TextProcess
{
	enum LineBuilderDirectionEnum
	{
		Next,
		Prev
	};

	class ILineBuilder
	{
	public:
		DECLARE_PROPERTY(LineBuilderDirectionEnum, BuilderDirection);

	public:
		virtual int BuildLines() = 0;
		virtual void Cancel() = 0;
	};
}
