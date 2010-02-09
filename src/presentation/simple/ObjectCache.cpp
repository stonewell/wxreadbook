#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "ObjectCache.h"
#include "../../ReadBookApp.h"
#include "../ReadBookView.h"
#include "ReadBookChar.h"
#include "ReadBookLine.h"

#include <deque>

template<class T>
class CAllocator
{
public:
	virtual void InitRealObject(T * obj, va_list args) = 0;

	virtual void CreateRealObject(T * & pObj, va_list args) = 0;
	
	virtual void ReleaseRealObject(T * pObj) = 0;
};

template<class T>
class CObjectCache
{
public:
	CObjectCache(CAllocator<T> * pAllocator) :
	  m_pAllocator(pAllocator)
	{
	}

	virtual ~CObjectCache()
	{
		wxInt32 size = m_ObjectList.size();

		for(wxInt32 i = 0; i < size; i++)
		{
			T * p = m_ObjectList.front();

			m_ObjectList.pop_front();

			m_pAllocator->ReleaseRealObject(p);
		}
	}

	T * CreateObject(int arg_count, ...)
	{
		va_list args;

		va_start(args, arg_count);

		if (m_ObjectList.size() > 0)
		{
			T * p = m_ObjectList.front();

			m_ObjectList.pop_front();

			m_pAllocator->InitRealObject(p, args);

			va_end(args);
	
			return p;
		}

		T * p = NULL;
		
		m_pAllocator->CreateRealObject(p, args);
		m_pAllocator->InitRealObject(p,args);

		va_end(args);

		return p;
	}

	void ReleaseObject(T * obj)
	{
		if (obj != NULL)
		{
			m_ObjectList.push_back(obj);
		}
	}

private:
	std::deque<T *> m_ObjectList;
	CAllocator<T> * m_pAllocator;
};

class CReadBookLineCache : public CObjectCache<CReadBookLine>,
	public CAllocator<CReadBookLine>
{
public:
	CReadBookLineCache() : CObjectCache<CReadBookLine>(this)
	{
	}

	virtual ~CReadBookLineCache()
	{
	}

protected:
	virtual void InitRealObject(CReadBookLine * obj, va_list args);
	virtual void CreateRealObject(CReadBookLine * & pObj, va_list args);
	virtual void ReleaseRealObject(CReadBookLine * pObj)
	{
		delete pObj;
	}
};

class CReadBookCharCache : public CObjectCache<CReadBookChar>,
	public CAllocator<CReadBookChar>
{
public:
	CReadBookCharCache() : CObjectCache<CReadBookChar>(this)
	{
	}

	virtual ~CReadBookCharCache()
	{
	}

protected:
	virtual void InitRealObject(CReadBookChar * obj, va_list args);
	virtual void CreateRealObject(CReadBookChar * & pChar, va_list args);
	virtual void ReleaseRealObject(CReadBookChar * pObj)
	{
		delete pObj;
	}
};

void CReadBookCharCache::InitRealObject(CReadBookChar * obj, va_list args)
{
	CReadBookView * pView = va_arg(args, CReadBookView *);

	obj->Init(pView);
}

void CReadBookCharCache::CreateRealObject(CReadBookChar * & pChar, va_list args)
{
	CReadBookView * pView = va_arg(args, CReadBookView *);

	pChar = new CReadBookChar(pView);
}

void CReadBookLineCache::InitRealObject(CReadBookLine * obj, va_list args)
{
	CReadBookView * pView = va_arg(args, CReadBookView *);
	wxInt32 nLineWidth = va_arg(args, wxInt32);
	wxInt32 nMaxAsciiCharCount = va_arg(args, wxInt32);
	wxInt32 nAvgAsciiCharWidth = va_arg(args, wxInt32);
	wxInt32 nColMargin = va_arg(args, wxInt32);

	obj->Init(pView, nLineWidth, nMaxAsciiCharCount,
		nAvgAsciiCharWidth, nColMargin);
}

void CReadBookLineCache::CreateRealObject(CReadBookLine * & pLine, va_list args)
{
	CReadBookView * pView = va_arg(args, CReadBookView *);
	wxInt32 nLineWidth = va_arg(args, wxInt32);
	wxInt32 nMaxAsciiCharCount = va_arg(args, wxInt32);
	wxInt32 nAvgAsciiCharWidth = va_arg(args, wxInt32);
	wxInt32 nColMargin = va_arg(args, wxInt32);

	pLine = new CReadBookLine(pView, nLineWidth, nMaxAsciiCharCount,
		nAvgAsciiCharWidth, nColMargin);
}

static CReadBookLineCache * g_pReadBookLineCache = NULL;
static CReadBookCharCache * g_pReadBookCharCache = NULL;

CReadBookLine * CreateReadBookLine(CReadBookView * pView,
							 wxInt32 nLineWidth,
							 wxInt32 nMaxAsciiCharCount,
							 wxInt32 nAvgAsciiCharWidth,
							 wxInt32 nColMargin)
{
	return g_pReadBookLineCache->CreateObject(5, pView,
		nLineWidth, nMaxAsciiCharCount, nAvgAsciiCharWidth,
		nColMargin);
}

void ReleaseReadBookLine(CReadBookLine * pLine)
{
	g_pReadBookLineCache->ReleaseObject(pLine);
}

CReadBookChar * CreateReadBookChar(CReadBookView * pView)
{
	return g_pReadBookCharCache->CreateObject(1, pView);
}

void ReleaseReadBookChar(CReadBookChar * pChar)
{
	g_pReadBookCharCache->ReleaseObject(pChar);
}

void InitObjectCache()
{
	g_pReadBookLineCache = new CReadBookLineCache();
	g_pReadBookCharCache = new CReadBookCharCache();
}

void ReleaseObjectCache()
{
	if (g_pReadBookLineCache != NULL)
		delete g_pReadBookLineCache;
	g_pReadBookLineCache = NULL;

	if (g_pReadBookCharCache != NULL)
		delete g_pReadBookCharCache;
	g_pReadBookCharCache = NULL;
}