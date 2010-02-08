#pragma once

namespace TextProcess
{
    namespace IO
    {
        class IMemoryMappedFile
        {
        public:
            static IMemoryMappedFile * CreateMemoryMappedFile(const wxString & strFileName, wxMBConv * pEncoding);

			virtual const wxByte * GetBuffer() = 0;
			virtual wxFileOffset GetLength() const = 0;
			virtual wxChar * DecodeData(int nOffset, int nLength) = 0;

			DECLARE_PROPERTY(wxMBConv *, Encoding);
        };
    }
}
