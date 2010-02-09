#include "nsVerifier.h"

CnsVerifier::CnsVerifier(void) :
	m_cclass(NULL),
	m_states(NULL)
{
}

CnsVerifier::~CnsVerifier(void)
{
	if (NULL != m_states) delete m_states;
	if (NULL != m_cclass) delete m_cclass;
}

char CnsVerifier::GetNextState(CnsVerifier * pV, char b, char s)
{
	CnsVerifier & v = *pV;

	return (char) ( 0xFF & 
		(((v.states()[((
		(s*v.stFactor()+(((v.cclass()[((b&0xFF)>>v.eidxSft4bits)]) 
		>> ((b & v.eSftMsk4bits) << v.eBitSft4bits)) 
		& v.eUnitMsk4bits ))&0xFF)
		>> v.eidxSft4bits) ]) >> (((
		(s*v.stFactor()+(((v.cclass()[((b&0xFF)>>v.eidxSft4bits)]) 
		>> ((b & v.eSftMsk4bits) << v.eBitSft4bits)) 
		& v.eUnitMsk4bits ))&0xFF) 
		& v.eSftMsk4bits) << v.eBitSft4bits)) & v.eUnitMsk4bits )
		) ;
}

