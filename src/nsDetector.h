#pragma once

#include "nsPSMDetector.h"
#include "nsICharsetDetectionObserver.h"
#include "nsICharsetDetector.h"

class CnsDetector : public
	CnsPSMDetector, CnsICharsetDetector
{
public:
	CnsDetector(void);
	CnsDetector(int langFlag);
	virtual ~CnsDetector(void);

private:
	CnsICharsetDetectionObserver * mObserver;

public:
	void Init(CnsICharsetDetectionObserver * aObserver);
	bool DoIt(char * aBuf, int aLen, bool oDontFeedMe);
	void Done();
	void Report(String charset);
	bool isAscii(const char * aBuf, int aLen);
};
