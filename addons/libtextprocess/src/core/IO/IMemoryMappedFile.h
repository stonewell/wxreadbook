#pragma once

namespace TextProcess
{
    namespace IO
    {
        class IMemoryMappedFile
        {
        public:
            static IMemoryMappedFile * CreateMemoryMappedFile(const wxString & strFileName);

			virtual const wxByte * GetBuffer() = 0;
			virtual wxFileOffset GetLength() const = 0;
        };
    }
}
