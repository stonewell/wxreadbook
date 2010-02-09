#pragma once

#include "nsVerifier.h"
#include "nsEUCStatistics.h"
#include "nsEUCSampler.h"

#include <vector>

typedef std::vector<String> StringVector;

class CnsPSMDetector
{
private:
	static CnsVerifier * TRADITIONAL_CHINESE_VERIFIER[];
	static CnsEUCStatistics * TRADITIONAL_CHINESE_STATISTICS[];
	static CnsVerifier * KOREAN_VERIFIER[];
	static CnsEUCStatistics * KOREAN_STATISTICS[];
	static CnsVerifier * SIMPLIFIED_CHINESE_VERIFIER[];
	static CnsEUCStatistics * SIMPLIFIED_CHINESE_STATISTICS[];
	static CnsVerifier * JAPANESE_VERIFIER[];
	static CnsEUCStatistics * JAPANESE_STATISTICS[];
	static CnsVerifier * CHINESE_VERIFIER[];
	static CnsEUCStatistics * CHINESE_STATISTICS[];
	static CnsVerifier * ALL_VERIFIER[];
	static CnsEUCStatistics * ALL_STATISTICS[];

public:
	static const int ALL                 =  0 ;
	static const int JAPANESE            =  1 ;
	static const int CHINESE             =  2 ;
	static const int SIMPLIFIED_CHINESE  =  3 ;
	static const int TRADITIONAL_CHINESE =  4 ;
	static const int KOREAN              =  5 ;

	static const int NO_OF_LANGUAGES     =  6 ;
	static const int MAX_VERIFIERS       = 16 ;

public:
	CnsPSMDetector(void);
	CnsPSMDetector(int langFlag);
	CnsPSMDetector(int aItems, CnsVerifier** aVerifierSet, 
		CnsEUCStatistics** aStatisticsSet);
	virtual ~CnsPSMDetector(void);

protected:
	CnsVerifier ** mVerifier;
	CnsEUCStatistics ** mStatisticsData;

	CnsEUCSampler	mSampler;
	char * mState;
	int * mItemIdx;

	int     mItems ;
	int	   mClassItems ;

	bool mDone ;
	bool mRunSampler ;
	bool mClassRunSampler ;

	virtual void InitVerifiers(int currVerSet);

public:
	virtual void Report(String charset) = 0;
	virtual void Reset();
	virtual bool HandleData(char * aBuf, int len);
	virtual void DataEnd();
	virtual void Sample(char * aBuf, int aLen);
	virtual void Sample(char * aBuf, int aLen, bool aLastChance);
	virtual void GetProbableCharsets(StringVector & ppCharsets);

};
