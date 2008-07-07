#pragma once

class CnsEUCSampler
{
public:
	CnsEUCSampler(void);
	virtual ~CnsEUCSampler(void);

private:
	int mTotal;
	int mThreshold;
	int mState;
	int * mFirstByteCnt;
	int * mSecondByteCnt;
	float * mFirstByteFreq;
	float * mSecondByteFreq;

public:
	void Reset();
	bool EnoughData();
	bool GetSomeData();
	bool Sample(char* aIn, int aLen) ;
	void CalFreq();
	float GetScore(float* aFirstByteFreq, float aFirstByteWeight,
								  float* aSecondByteFreq, float aSecondByteWeight);
	float GetScore(float* array1, float* array2);
};
