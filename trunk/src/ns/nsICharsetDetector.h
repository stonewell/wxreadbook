#pragma once

class CnsICharsetDetector {

public:
	virtual void Init(CnsICharsetDetectionObserver * observer) = 0 ;
	virtual bool DoIt(char * aBuf, int aLen, bool oDontFeedMe) = 0 ;
	virtual void Done() = 0;
};

