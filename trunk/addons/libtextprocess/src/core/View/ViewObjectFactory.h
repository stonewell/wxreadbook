#pragma once

namespace TextProcess
{
	namespace View
	{
		class CViewObjectFactory
		{
		private:
			CViewObjectFactory() {}

		public:
			static IViewLine * CreateViewLine(int viewLineOffset, int viewLineSize);
			static IViewLineMatcher * CreateLineMatcher(int docOffset, int viewOffset);
			static IViewLineManager * CreateLineManager();
			static IViewLineBuilder * CreateLineBuilder();
		};
	}
}
