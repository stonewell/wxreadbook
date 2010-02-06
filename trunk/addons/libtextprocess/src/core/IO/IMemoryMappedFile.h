#pragma once

namespace TextProcess
{
    namespace IO
    {
        class IMemoryMappedFile
        {
        public:
            static IMemoryMappedFile * CreateMemoryMappedFile(const wxString & strFileName);

			const wxByte * GetBuffer() = 0;
			wxFileOffset GetLength() const = 0;
        };
    }
}
