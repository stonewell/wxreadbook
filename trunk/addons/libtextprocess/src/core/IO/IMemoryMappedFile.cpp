#include "../TextProcess.h"
#include "../Impl/TextProcessImpl.h"

TextProcess::IO::IMemoryMappedFile * TextProcess::IO::IMemoryMappedFile::CreateMemoryMappedFile(const wxString & strFileName, wxMBConv * pEncoding)
{
    return new TextProcess::IO::Impl::CMemoryMappedFile(strFileName, pEncoding);
}

