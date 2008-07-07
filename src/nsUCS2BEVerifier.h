#pragma once
#include "nsVerifier.h"

class CnsUCS2BEVerifier :
	public CnsVerifier
{
public:
	CnsUCS2BEVerifier(void);
	virtual ~CnsUCS2BEVerifier(void);

	virtual bool isUCS2() { return true; }
};

