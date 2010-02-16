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
		DECLARE_TPL_INTERFACE(ILine);
		DECLARE_PROPERTY_GETTER(LineTypeEnum, Type);

		DECLARE_PROPERTY(wxFileOffset, Offset);
		DECLARE_PROPERTY(wxFileOffset, Length);
		DECLARE_PROPERTY(wxUint32, Index);
		
		virtual int WaitForAccessing(int timeout = INFINITE) = 0;
		virtual void AccessLine() = 0;
		virtual void ClearAccess() = 0;
	};
}
