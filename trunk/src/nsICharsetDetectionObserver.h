#pragma once

#include "nsString.h"

class CnsICharsetDetectionObserver
{
public:
	CnsICharsetDetectionObserver(void);
	virtual ~CnsICharsetDetectionObserver(void);

	virtual void Notify(String charset) = 0;
};
