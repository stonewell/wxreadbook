#pragma once
#include "nsVerifier.h"

class CnsUCS2LEVerifier :
	public CnsVerifier
{
public:
	CnsUCS2LEVerifier(void);
	virtual ~CnsUCS2LEVerifier(void);

	virtual bool isUCS2() { return true; }
};

