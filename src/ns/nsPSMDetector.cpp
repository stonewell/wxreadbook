#include "nsPSMDetector.h"

#include "nsVerifier.h"
#include "nsUTF8Verifier.h"
#include "nsSJISVerifier.h"
#include "nsEUCJPVerifier.h"
#include "nsISO2022JPVerifier.h"
#include "nsEUCKRVerifier.h"
#include "nsISO2022KRVerifier.h"
#include "nsBIG5Verifier.h"
#include "nsEUCTWVerifier.h"
#include "nsGB2312Verifier.h"
#include "nsGB18030Verifier.h"
#include "nsISO2022CNVerifier.h"
#include "nsHZVerifier.h"
#include "nsCP1252Verifier.h"
#include "nsUCS2BEVerifier.h"
#include "nsUCS2LEVerifier.h"

#include "nsEUCJPStatistics.h"
#include "nsEUCKRStatistics.h"
#include "nsBIG5Statistics.h"
#include "nsEUCTWStatistics.h"
#include "nsGB2312Statistics.h"

static CnsUTF8Verifier UTF8Instance;
static CnsSJISVerifier SJISInstance;
static CnsEUCJPVerifier EUCJPVInstance;
static CnsISO2022JPVerifier ISO2022JPInstance;
static CnsEUCKRVerifier EUCKRVInstance;
static CnsISO2022KRVerifier ISO2022KRInstance;
static CnsBIG5Verifier BIG5VInstance;
static CnsEUCTWVerifier EUCTWVInstance;
static CnsGB2312Verifier GB2312VInstance;
static CnsGB18030Verifier GB18030Instance;
static CnsISO2022CNVerifier ISO2022CNInstance;
static CnsHZVerifier HZInstance;
static CnsCP1252Verifier CP1252Instance;
static CnsUCS2BEVerifier UCS2BEInstance;
static CnsUCS2LEVerifier UCS2LEInstance;

static CEUCJPStatistics EUCJPInstance;
static CEUCKRStatistics EUCKRInstance;
static CBig5Statistics Big5Instance;
static CEUCTWStatistics EUCTWInstance;
static CGB2312Statistics GB2312Instance;

CnsVerifier * CnsPSMDetector::TRADITIONAL_CHINESE_VERIFIER[] = {
	&UTF8Instance,
	&BIG5VInstance,
	&ISO2022CNInstance,
	&EUCTWVInstance,
	&CP1252Instance,
	&UCS2BEInstance,
	&UCS2LEInstance
};
CnsEUCStatistics * CnsPSMDetector::TRADITIONAL_CHINESE_STATISTICS[] = {
	NULL,
	&Big5Instance,
	NULL,
	&EUCTWInstance,
	NULL,
	NULL,
	NULL
};
CnsVerifier * CnsPSMDetector::KOREAN_VERIFIER[] = {
	&UTF8Instance,
	&EUCKRVInstance,
	&ISO2022KRInstance,
	&CP1252Instance,
	&UCS2BEInstance,
	&UCS2LEInstance
};
CnsVerifier * CnsPSMDetector::SIMPLIFIED_CHINESE_VERIFIER[] = {
	&UTF8Instance,
	&GB2312VInstance,
	&GB18030Instance,
	&ISO2022CNInstance,
	&HZInstance,
	&CP1252Instance,
	&UCS2BEInstance,
	&UCS2LEInstance
};
CnsVerifier * CnsPSMDetector::JAPANESE_VERIFIER[] = {
	&UTF8Instance,
	&SJISInstance,
	&EUCJPVInstance,
	&ISO2022JPInstance,
	&CP1252Instance,
	&UCS2BEInstance,
	&UCS2LEInstance
};
CnsVerifier * CnsPSMDetector::CHINESE_VERIFIER[] = {
	&UTF8Instance,
	&GB2312VInstance,
	&GB18030Instance,
	&BIG5VInstance,
	&ISO2022CNInstance,
	&HZInstance,
	&EUCTWVInstance,
	&CP1252Instance,
	&UCS2BEInstance,
	&UCS2LEInstance
};
CnsEUCStatistics * CnsPSMDetector::CHINESE_STATISTICS[] = {
	NULL,
	&GB2312Instance,
	NULL,
	&Big5Instance,
	NULL,
	NULL,
	&EUCTWInstance,
	NULL,
	NULL,
	NULL
};
CnsVerifier * CnsPSMDetector::ALL_VERIFIER[] = {
	&UTF8Instance,
	&SJISInstance,
	&EUCJPVInstance,
	&ISO2022JPInstance,
	&EUCKRVInstance,
	&ISO2022KRInstance,
	&BIG5VInstance,
	&EUCTWVInstance,
	&GB2312VInstance,
	&GB18030Instance,
	&ISO2022CNInstance,
	&HZInstance,
	&CP1252Instance,
	&UCS2BEInstance,
	&UCS2LEInstance
};
CnsEUCStatistics * CnsPSMDetector::ALL_STATISTICS[] = {
	NULL,
	NULL,
	&EUCJPInstance,
	NULL,
	&EUCKRInstance,
	NULL,
	&Big5Instance,
	&EUCTWInstance,
	&GB2312Instance,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

CnsPSMDetector::CnsPSMDetector(void)
{
	InitVerifiers( CnsPSMDetector::ALL );
	Reset() ;
}

CnsPSMDetector::CnsPSMDetector(int langFlag) 
{
	InitVerifiers(langFlag);
	Reset() ;
}

CnsPSMDetector::CnsPSMDetector(int aItems, CnsVerifier** aVerifierSet, 
							   CnsEUCStatistics** aStatisticsSet)  
{
	mClassRunSampler = ( aStatisticsSet != NULL ) ;
	mStatisticsData = aStatisticsSet ;
	mVerifier = aVerifierSet ;
	mClassItems = aItems ;
	Reset() ;
}

CnsPSMDetector::~CnsPSMDetector(void)
{
	delete mState;
	delete mItemIdx;
}

void CnsPSMDetector::Reset() {
	mState = new char[MAX_VERIFIERS];
	mItemIdx = new int[MAX_VERIFIERS];

	mRunSampler = mClassRunSampler ;
	mDone = false ;
	mItems = mClassItems ;
	for(int i=0; i<mItems; i++) {
		mState[i] = 0;
		mItemIdx[i] = i;
	}
	mSampler.Reset() ;
}

void CnsPSMDetector::InitVerifiers(int currVerSet) {
	int idx = 0 ;
	int currVerifierSet;

	if (currVerSet >=0 && currVerSet < NO_OF_LANGUAGES ) {
		currVerifierSet = currVerSet ;
	}
	else {
		currVerifierSet = CnsPSMDetector::ALL ;
	}

	mVerifier = NULL ;
	mStatisticsData = NULL ;
	if ( currVerifierSet == CnsPSMDetector::TRADITIONAL_CHINESE ) {
		mVerifier = TRADITIONAL_CHINESE_VERIFIER;
		mStatisticsData = TRADITIONAL_CHINESE_STATISTICS;
		mClassItems = sizeof(TRADITIONAL_CHINESE_VERIFIER) / sizeof(CnsVerifier*);
	}
	//==========================================================
	else if ( currVerifierSet == CnsPSMDetector::KOREAN ) {
		mVerifier = KOREAN_VERIFIER;
		mClassItems = sizeof(KOREAN_VERIFIER) / sizeof(CnsVerifier*);
	}
	//==========================================================
	else if ( currVerifierSet == CnsPSMDetector::SIMPLIFIED_CHINESE ) {
		mVerifier = SIMPLIFIED_CHINESE_VERIFIER;
		mClassItems = sizeof(SIMPLIFIED_CHINESE_VERIFIER) / sizeof(CnsVerifier*);
	}
	//==========================================================
	else if ( currVerifierSet == CnsPSMDetector::JAPANESE ) {
		mVerifier = JAPANESE_VERIFIER;
		mClassItems = sizeof(JAPANESE_VERIFIER) / sizeof(CnsVerifier*);
	}
	//==========================================================
	else if ( currVerifierSet == CnsPSMDetector::CHINESE ) {
		mVerifier = CHINESE_VERIFIER;
		mStatisticsData = CHINESE_STATISTICS;
		mClassItems = sizeof(CHINESE_VERIFIER) / sizeof(CnsVerifier*);
	}
	//==========================================================
	else if ( currVerifierSet == CnsPSMDetector::ALL ) {
		mVerifier = ALL_VERIFIER;
		mStatisticsData = ALL_STATISTICS;
		mClassItems = sizeof(ALL_VERIFIER) / sizeof(CnsVerifier*);
	}

	mClassRunSampler = ( mStatisticsData != NULL ) ;
}

bool CnsPSMDetector::HandleData(char * aBuf, int len) {
	int i,j;
	char b, st;
	for( i=0; i < len; i++) {
		b = aBuf[i] ;
		for (j=0; j < mItems; )
		{
			st = CnsVerifier::GetNextState( mVerifier[mItemIdx[j]], 
				b, mState[j]) ;
			if (st == CnsVerifier::eItsMe) {
				Report( mVerifier[mItemIdx[j]]->charset() );
				mDone = true ;
				return mDone ;
			} else if (st == CnsVerifier::eError ) {
				mItems--;
				if (j < mItems ) {
					mItemIdx[j] = mItemIdx[mItems];	
					mState[j]   = mState[mItems];
				}
			} else {
				mState[j++] = st ;
			}
		}
		if ( mItems <= 1 ) {
			if( 1 == mItems) {
				Report( mVerifier[mItemIdx[0]]->charset() );
			}
			mDone = true ;
			return mDone ;
		} 
		else {
			int nonUCS2Num=0;
			int nonUCS2Idx=0;
			for(j=0; j<mItems; j++) {
				if ( (!(mVerifier[mItemIdx[j]]->isUCS2())) &&
					(!(mVerifier[mItemIdx[j]]->isUCS2())) ) 
				{
					nonUCS2Num++ ;
					nonUCS2Idx = j ;
				}
			}
			if (1 == nonUCS2Num) {
				Report( mVerifier[mItemIdx[nonUCS2Idx]]->charset() );
				mDone = true ;
				return mDone ;
			}
		}
	} // End of for( i=0; i < len ...
	if (mRunSampler)
		Sample(aBuf, len);
	return mDone ;
}

void CnsPSMDetector::DataEnd() {
	if (mDone == true)
		return ;
	if (mItems == 2) {
		if ((mVerifier[mItemIdx[0]]->charset()).compare(_T("GB18030")) == 0) {
			Report(mVerifier[mItemIdx[1]]->charset()) ;
			mDone = true ;
		} else if ((mVerifier[mItemIdx[1]]->charset()).compare(_T("GB18030")) == 0) {
			Report(mVerifier[mItemIdx[0]]->charset()) ;
			mDone = true ;
		}
	}
	if (mRunSampler)
		Sample(NULL, 0, true);
}

void CnsPSMDetector::Sample(char * aBuf, int aLen) {
	Sample(aBuf, aLen, false) ;
}

void CnsPSMDetector::Sample(char * aBuf, int aLen, bool aLastChance)
{
	int possibleCandidateNum  = 0;
	int j;
	int eucNum=0 ;
	for (j=0; j< mItems; j++) {
		if (NULL != mStatisticsData[mItemIdx[j]]) 
			eucNum++ ;
		if ((!mVerifier[mItemIdx[j]]->isUCS2()) && 
			(!(mVerifier[mItemIdx[j]]->charset().compare(_T("GB18030")) == 0)))
			possibleCandidateNum++ ;
	}
	mRunSampler = (eucNum > 1) ;
	if (mRunSampler) {
		mRunSampler = mSampler.Sample(aBuf, aLen);
		if(((aLastChance && mSampler.GetSomeData()) || 
			mSampler.EnoughData())
			&& (eucNum == possibleCandidateNum)) {
				mSampler.CalFreq();
				int bestIdx = -1;
				int eucCnt=0;
				float bestScore = 0.0f;
				for(j = 0; j < mItems; j++) {
					if((NULL != mStatisticsData[mItemIdx[j]])  &&
						(!(mVerifier[mItemIdx[j]]->charset().compare(_T("Big5")) == 0)))
					{
						float score = mSampler.GetScore(
							mStatisticsData[mItemIdx[j]]->mFirstByteFreq(),
							mStatisticsData[mItemIdx[j]]->mFirstByteWeight(),
							mStatisticsData[mItemIdx[j]]->mSecondByteFreq(),
							mStatisticsData[mItemIdx[j]]->mSecondByteWeight() );
						//System.out.println("FequencyScore("+mVerifier[mItemIdx[j]].charset()+")= "+ score);
						if(( 0 == eucCnt++) || (bestScore > score )) {
							bestScore = score;
							bestIdx = j;
						} // if(( 0 == eucCnt++) || (bestScore > score )) 
					} // if(NULL != ...)
				} // for
				if (bestIdx >= 0)
				{
					Report( mVerifier[mItemIdx[bestIdx]]->charset());
					mDone = true;
				}
		} // if (eucNum == possibleCandidateNum)
	} // if(mRunSampler)
}

void CnsPSMDetector::GetProbableCharsets(StringVector & ppCharsets) {
	if (mItems <= 0) {
		ppCharsets.clear();
		return;
	}
	for (int i=0; i<mItems; i++)
		ppCharsets.push_back(mVerifier[mItemIdx[i]]->charset()) ;
}

