#pragma once

class CnsEUCStatistics
{
public:
	CnsEUCStatistics(void);
	virtual ~CnsEUCStatistics(void);

public:
	virtual float * mFirstByteFreq() { return m_FirstByteFreq; } 
	virtual float mFirstByteStdDev() { return m_FirstByteStdDev; } 
	virtual float mFirstByteMean() { return m_FirstByteMean; } 
	virtual float mFirstByteWeight() { return m_FirstByteWeight; } 
	virtual float * mSecondByteFreq() { return m_SecondByteFreq; } 
	virtual float mSecondByteStdDev() { return m_SecondByteStdDev; } 
	virtual float mSecondByteMean() { return m_SecondByteMean; } 
	virtual float mSecondByteWeight() { return m_SecondByteWeight; } 

protected:
	float * m_FirstByteFreq ;
	float m_FirstByteStdDev;
	float m_FirstByteMean;
	float m_FirstByteWeight;
	float * m_SecondByteFreq;
	float m_SecondByteStdDev;
	float m_SecondByteMean;
	float m_SecondByteWeight;
};
