#include "nsDetector.h"

CnsDetector::CnsDetector(void) :
mObserver(NULL)
{
}

CnsDetector::CnsDetector(int langFlag) : CnsPSMDetector(langFlag) ,
mObserver(NULL)
{
}

CnsDetector::~CnsDetector(void)
{
}

void CnsDetector::Init(CnsICharsetDetectionObserver * aObserver) 
{
	mObserver = aObserver ;
}

bool CnsDetector::DoIt(char * aBuf, int aLen, bool oDontFeedMe) 
{

	if (aBuf == NULL || oDontFeedMe )
		return false ;

	HandleData(aBuf, aLen) ;	
	return mDone ;
}

void CnsDetector::Done() 
{
	DataEnd() ;
	return ;
}

void CnsDetector::Report(String charset) 
{
	if (mObserver != NULL)
		mObserver->Notify(charset)  ;
}

bool CnsDetector::isAscii(const char * aBuf, int aLen) 
{
	for(int i=0; i<aLen; i++) {
		if ((0x0080 & aBuf[i]) != 0) {
			return false ;
		}
	}
	return true ;
}
