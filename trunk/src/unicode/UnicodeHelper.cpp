// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "UnicodeHelper.h"
#include "../ReadBookIds.h"

static wxInt32 VALID_UNICODE[][2] =
{
	{0x0000, 0x007F},
	{0x0080, 0x00FF},
	{0x3000, 0x303F},
	{0x2000, 0x206F},
	{0x2E00, 0x2E7F},
	{0xFF00, 0xFFEF},
	{0x20A0, 0x20CF},
	{0x4E00, 0x9FBF},
	{0x3400, 0x4DBF},
	{0x20000, 0x2A6DF},
	{0xF900, 0xFAFF},
	{0x2F800, 0x2FA1F},
	{0x3190, 0x319F},
	{0x25A0, 0x25FF},
	{0x2460, 0x24FF},
	{0x2600, 0x26FF},
	{0x3100, 0x312F},
	{0x0300, 0x036F}
};

static wxInt32 INVALID_UINICODE[][2] = 
{
	{0xE000, 0xF8FF},
	{0xF0000, 0xFFFFD},
	{0x100000, 0x10FFFD},
	{0xD800, 0xDBFF},
	{0xDC00, 0xDFF},
	{0xFFF0, 0xFFFF},
	{0x1FF80, 0x1FFFF},
	{0x2FF80, 0x2FFFF},
	{0x3FF80, 0x3FFFF},
	{0x4FF80, 0x4FFFF},
	{0x5FF80, 0x5FFFF},
	{0x6FF80, 0x6FFFF},
	{0x7FF80, 0x7FFFF},
	{0x8FF80, 0x8FFFF},
	{0x9FF80, 0x9FFFF},
	{0xAFF80, 0xAFFFF},
	{0xBFF80, 0xBFFFF},
	{0xCFF80, 0xCFFFF},
	{0xDFF80, 0xDFFFF},
	{0xEFF80, 0xEFFFF},
	{0xFFF80, 0xFFFFF},
	{0x10FF80, 0x10FFFF}
};

bool IsValidUnicode(wxChar ch)
{
	//Invalid Chars
	int size = sizeof(INVALID_UINICODE) / 2 / sizeof(wxInt32);

	for(int i = 0; i < size; i++)
	{
		if (ch >= INVALID_UINICODE[i][0] && ch <= INVALID_UINICODE[i][1])
		{
			return false;
		}
	}

	//Valid Chars
	size = sizeof(VALID_UNICODE) / 2 / sizeof(wxInt32);

	for(int i = 0; i < size; i++)
	{
		if (ch >= VALID_UNICODE[i][0] && ch <= VALID_UNICODE[i][1])
		{
			return true;
		}
	}

	return true;
}

#if wxUSE_UNICODE
#include "../ns/nsDetector.h"

class CObserver : public CnsICharsetDetectionObserver
{
public:
	bool m_bDone;
	String m_Charset;

	CObserver() :
	m_bDone(false),
		m_Charset(wxT(""))
	{
	}

	virtual void Notify(String charset) 
	{
		if (!m_bDone)
		{
			m_Charset = charset;
			m_bDone = true;
		}
	}
};
#endif

bool GuessDataEncoding(wxInputStream * pInput, wxString & charsets, int lang)
{
	CObserver ob;

	int detLang = CnsPSMDetector::CHINESE;

	switch(lang)
	{
	case IDM_LANG_JAPANESE:
		detLang = CnsPSMDetector::JAPANESE;
		break;
	case IDM_LANG_KOREAN:
		detLang = CnsPSMDetector::KOREAN;
		break;
	case IDM_LANG_CHINESE:
	default:
		break;
	}
	CnsDetector det(detLang);

	det.Init(&ob);

	char buf[1024] = {0};

	bool done = false;
	bool isAscii = false;

	while(!done && !ob.m_bDone && !pInput->Eof())
	{
		pInput->Read(buf, 1024);

		size_t count = pInput->LastRead();

		isAscii = det.isAscii(buf, count);
		
		if (!isAscii && !done)
			done = det.DoIt(buf, count, false);
	}
	det.DataEnd();

	StringVector ppCharsets;
	det.GetProbableCharsets(ppCharsets);

	if (ppCharsets.size() > 0 || ob.m_bDone)
	{
		charsets = ob.m_bDone ? ob.m_Charset : ppCharsets[0];

		return true;
	}
	else if (isAscii)
	{
		charsets = wxT("GB18030");
		return true;
	}
	else
	{
		charsets = wxT("GB18030");
		return true;
	}
}

