#pragma once

#include "nsString.h"

class CnsVerifier
{
public:
	static const char eStart = (char)0;
	static const char eError = (char)1;
	static const char eItsMe = (char)2;
	static const int eidxSft4bits = 3;
	static const int eSftMsk4bits = 7;
	static const int eBitSft4bits = 2;
	static const int eUnitMsk4bits = 0x0000000F;

public:
	CnsVerifier();
	virtual ~CnsVerifier();

	virtual int*  cclass()   { return m_cclass ;   }
	virtual int*  states()   { return m_states ;   }
	virtual int    stFactor() { return m_stFactor ; }
	virtual String charset()  { return m_charset ;  }

	virtual bool isUCS2() { return false; }

	static char GetNextState(CnsVerifier * pv, char b, char s);

protected:
	 int*  m_cclass   ; 
	 int*  m_states   ; 
	 int    m_stFactor ; 
	 String m_charset  ; 
};
