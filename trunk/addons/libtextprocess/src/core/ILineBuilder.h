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
		DECLARE_TPL_INTERFACE(ILineBuilder);
		DECLARE_PROPERTY(LineBuilderDirectionEnum, BuilderDirection);
		DECLARE_PROPERTY(wxUint32, BuildLineCount);
		DECLARE_PROPERTY(int, WaitForLineAccessed);

	public:
		virtual int BuildLines() = 0;
		virtual void Cancel() = 0;
	};
}
