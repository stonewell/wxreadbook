#include "nsEUCSampler.h"
#include <complex>

CnsEUCSampler::CnsEUCSampler(void)
{
	mTotal = 0;
	mThreshold = 200 ;
	mState = 0;
	mFirstByteCnt = new int[94] ;
	mSecondByteCnt = new int[94] ;
	mFirstByteFreq = new float[94] ;
	mSecondByteFreq = new float[94];

	Reset() ;
}

CnsEUCSampler::~CnsEUCSampler(void)
{
	delete mFirstByteCnt;
	delete mSecondByteCnt;
	delete mFirstByteFreq;
	delete mSecondByteFreq;
}

void CnsEUCSampler::Reset() 
{
	mTotal = 0 ;
	mState = 0 ;
	for(int i=0; i<94; i++)
		mFirstByteCnt[i] = mSecondByteCnt[i] = 0 ;
}

bool CnsEUCSampler::EnoughData() { return mTotal > mThreshold; }

bool CnsEUCSampler::GetSomeData() { return mTotal > 1; }

bool CnsEUCSampler::Sample(char* aIn, int aLen) 
{

	if(mState == 1)
		return false;

	int p = 0;

	int i;
	for(i=0; (i<aLen) && (1 != mState) ;i++,p++)
	{
		switch(mState) 
		{
		case 0:
			if( ( aIn[p] & 0x0080 ) != 0 ) 
			{
				if((0xff==(0xff&aIn[p])) || ( 0xa1>(0xff&aIn[p]))) 
				{
					mState = 1;
				} 
				else 
				{
					mTotal++;
					mFirstByteCnt[(0xff&aIn[p]) - 0xa1]++;
					mState = 2;
				}
			}
			break;
		case 1:
			break;
		case 2:
			if( (aIn[p] & 0x0080) != 0 )
			{
				if((0xff == (0xff&aIn[p])) 
					|| ( 0xa1 > (0xff&aIn[p]))) 
				{
					mState = 1;
				} 
				else
				{
					mTotal++;
					mSecondByteCnt[(0xff&aIn[p]) - 0xa1]++;
					mState = 0;
				}
			} 
			else
			{
				mState = 1;
			}
			break;
		default:
			mState = 1;
		}
	}

	return (1 != mState );
}


void CnsEUCSampler::CalFreq() 
{
	for(int i = 0 ; i < 94; i++) 
	{
		mFirstByteFreq[i] = (float)mFirstByteCnt[i] / (float)mTotal;
		mSecondByteFreq[i] = (float)mSecondByteCnt[i] / (float)mTotal;
	}
}

float CnsEUCSampler::GetScore(float* aFirstByteFreq, float aFirstByteWeight,
							  float* aSecondByteFreq, float aSecondByteWeight)
{
	return aFirstByteWeight * GetScore(aFirstByteFreq, mFirstByteFreq) +
		aSecondByteWeight * GetScore(aSecondByteFreq, mSecondByteFreq);
}

float CnsEUCSampler::GetScore(float* array1, float* array2) 
{
	float s;
	float sum=0.0f;

	for(int i=0;i<94;i++) 
	{
		s = array1[i] - array2[i];
		sum += s * s;
	}

	return (float)std::sqrt((double)sum) / 94.0f;
}

