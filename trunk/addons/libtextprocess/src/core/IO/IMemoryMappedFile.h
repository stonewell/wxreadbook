#pragma once

namespace TextProcess
{
    namespace IO
    {
        class IMemoryMappedFile
        {
        public:
			DECLARE_TPL_INTERFACE(IMemoryMappedFile);

			static IMemoryMappedFile * CreateMemoryMappedFile(const wxString & strFileName, wxMBConv * pEncoding);
            static IMemoryMappedFile * CreateMemoryMappedFile(wxInputStream * pInput, wxMBConv * pEncoding);

			virtual const wxByte * GetBuffer() = 0;
			virtual wxFileOffset GetLength() const = 0;
			virtual wxChar * DecodeData(wxFileOffset nOffset, wxFileOffset nLength, wxInt32 & nDecodedSize) = 0;

			DECLARE_PROPERTY(wxMBConv *, Encoding);
        };
    }
}
