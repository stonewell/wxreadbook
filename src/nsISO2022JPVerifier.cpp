#include "nsISO2022JPVerifier.h"

CnsISO2022JPVerifier::~CnsISO2022JPVerifier(void)
{
}

CnsISO2022JPVerifier::CnsISO2022JPVerifier(void)
{

    m_cclass = new int[256/8] ;

    m_cclass[0] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (2))) )))))) ;
    m_cclass[1] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[2] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[3] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[4] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 7))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[5] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (3))) )))))) ;
    m_cclass[6] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[7] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[8] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (4))) ) << 8) | (   ((int)(((0) << 4) | (6))) )))))) ;
    m_cclass[9] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (5))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[10] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[11] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[12] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[13] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[14] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[15] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((0) << 4) | ( 0))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (0))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[16] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[17] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[18] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[19] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[20] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[21] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[22] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[23] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[24] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[25] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[26] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[27] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[28] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[29] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[30] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[31] = ((int)(((  ((int)(((  ((int)((( 2) << 4) | (2)))  ) << 8) | (((int)(((2) << 4) | ( 2))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;

    m_states = new int[6] ;

    m_states[0] = ((int)(((  ((int)(((  ((int)((( eStart) << 4) | (eStart)))  ) << 8) | (((int)(((eStart) << 4) | ( eStart))) ))) ) << 16) | (  ((int)(((  ((int)(((eStart) << 4) | (eError))) ) << 8) | (   ((int)(((     3) << 4) | (eStart))) )))))) ;
    m_states[1] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[2] = ((int)(((  ((int)(((  ((int)((( eItsMe) << 4) | (eItsMe)))  ) << 8) | (((int)(((eItsMe) << 4) | ( eItsMe))) ))) ) << 16) | (  ((int)(((  ((int)(((eItsMe) << 4) | (eItsMe))) ) << 8) | (   ((int)(((eItsMe) << 4) | (eItsMe))) )))))) ;
    m_states[3] = ((int)(((  ((int)(((  ((int)(((      4) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((     5) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[4] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eItsMe)))  ) << 8) | (((int)(((eError) << 4) | ( eItsMe))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[5] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eItsMe) << 4) | ( eItsMe))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;

    m_charset = _T("ISO-2022-JP");
    m_stFactor =  8;

}

