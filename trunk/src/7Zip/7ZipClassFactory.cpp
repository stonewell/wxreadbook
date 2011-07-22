// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/hashmap.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
#endif

#include "wx/datstrm.h"
#include "wx/mstream.h"
#include "wx/ptr_scpd.h"
#include "wx/wfstream.h"

#include "7ZipEntry.h"
#include "7ZipOutputStream.h"
#include "7ZipInputStream.h"
#include "7ZipClassFactory.h"

#include "lib7zip.h"

IMPLEMENT_DYNAMIC_CLASS(C7ZipClassFactory, wxArchiveClassFactory)

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(C7ZipClassFactoryPtrList)

/////////////////////////////////////////////////////////////////////////////
// Class factory

C7ZipClassFactory::C7ZipClassFactory(const wxString & strExt) : m_strExt(strExt)
{
    m_Protocols[0] = m_strExt;

	m_strMimeType = L"application/";
	m_strMimeType += m_strExt;

	m_MimeTypes[0] = m_strMimeType;

	m_strFileExt = L".";
	m_strFileExt += m_strExt;

	m_FileExts[0] = m_strFileExt;

	m_Protocols[1] = m_MimeTypes[1] = m_FileExts[1] = NULL; 

	PushFront();
}

const wxChar * const *
C7ZipClassFactory::GetProtocols(wxStreamProtocolType type) const
{
    static const wxChar *empty[]     = { NULL };

    switch (type) 
	{
        case wxSTREAM_PROTOCOL: return m_Protocols;
        case wxSTREAM_MIMETYPE: return m_MimeTypes;
        case wxSTREAM_FILEEXT:  return m_FileExts;
        default:                return empty;
    }
}

class C7ZipClassFactoryManager
{
public:
	C7ZipClassFactoryManager()
	{
		m_7ZipLibrary.Initialize();

		WStringArray exts;

		if (m_7ZipLibrary.GetSupportedExts(exts))
		{
			for(WStringArray::iterator it = exts.begin(); it != exts.end(); it++)
			{
#ifdef __WXMAC__
				wxString ext((*it).c_str());
#else
				wxString ext = *it;
#endif

				C7ZipClassFactory * pFactory = new C7ZipClassFactory(ext);

				m_FactoryList.push_back(pFactory);
			}
		}
	}

	~C7ZipClassFactoryManager()
	{
		m_7ZipLibrary.Deinitialize();

		WX_CLEAR_LIST(C7ZipClassFactoryPtrList, m_FactoryList);
	}

private:
	C7ZipLibrary m_7ZipLibrary;
	C7ZipClassFactoryPtrList m_FactoryList;
};

C7ZipClassFactoryManager * g_p7ZipClassFactoryManager = NULL;

void Initialize7ZipClassFactories()
{
	if (g_p7ZipClassFactoryManager == NULL)
		g_p7ZipClassFactoryManager = new C7ZipClassFactoryManager();
}

void Deinitialize7ZipClassFactories()
{
	 if (g_p7ZipClassFactoryManager != NULL)
		delete g_p7ZipClassFactoryManager;
}
