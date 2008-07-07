#include "nsUTF8Verifier.h"

CnsUTF8Verifier::~CnsUTF8Verifier(void)
{
}

CnsUTF8Verifier::CnsUTF8Verifier(void)
{

    m_cclass = new int[256/8] ;

    m_cclass[0] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[1] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[2] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[3] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((0) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[4] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[5] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[6] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[7] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[8] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[9] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[10] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[11] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[12] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[13] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[14] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[15] = ((int)(((  ((int)(((  ((int)((( 1) << 4) | (1)))  ) << 8) | (((int)(((1) << 4) | ( 1))) ))) ) << 16) | (  ((int)(((  ((int)(((1) << 4) | (1))) ) << 8) | (   ((int)(((1) << 4) | (1))) )))))) ;
    m_cclass[16] = ((int)(((  ((int)(((  ((int)((( 3) << 4) | (3)))  ) << 8) | (((int)(((3) << 4) | ( 3))) ))) ) << 16) | (  ((int)(((  ((int)(((2) << 4) | (2))) ) << 8) | (   ((int)(((2) << 4) | (2))) )))))) ;
    m_cclass[17] = ((int)(((  ((int)(((  ((int)((( 4) << 4) | (4)))  ) << 8) | (((int)(((4) << 4) | ( 4))) ))) ) << 16) | (  ((int)(((  ((int)(((4) << 4) | (4))) ) << 8) | (   ((int)(((4) << 4) | (4))) )))))) ;
    m_cclass[18] = ((int)(((  ((int)(((  ((int)((( 4) << 4) | (4)))  ) << 8) | (((int)(((4) << 4) | ( 4))) ))) ) << 16) | (  ((int)(((  ((int)(((4) << 4) | (4))) ) << 8) | (   ((int)(((4) << 4) | (4))) )))))) ;
    m_cclass[19] = ((int)(((  ((int)(((  ((int)((( 4) << 4) | (4)))  ) << 8) | (((int)(((4) << 4) | ( 4))) ))) ) << 16) | (  ((int)(((  ((int)(((4) << 4) | (4))) ) << 8) | (   ((int)(((4) << 4) | (4))) )))))) ;
    m_cclass[20] = ((int)(((  ((int)(((  ((int)((( 5) << 4) | (5)))  ) << 8) | (((int)(((5) << 4) | ( 5))) ))) ) << 16) | (  ((int)(((  ((int)(((5) << 4) | (5))) ) << 8) | (   ((int)(((5) << 4) | (5))) )))))) ;
    m_cclass[21] = ((int)(((  ((int)(((  ((int)((( 5) << 4) | (5)))  ) << 8) | (((int)(((5) << 4) | ( 5))) ))) ) << 16) | (  ((int)(((  ((int)(((5) << 4) | (5))) ) << 8) | (   ((int)(((5) << 4) | (5))) )))))) ;
    m_cclass[22] = ((int)(((  ((int)(((  ((int)((( 5) << 4) | (5)))  ) << 8) | (((int)(((5) << 4) | ( 5))) ))) ) << 16) | (  ((int)(((  ((int)(((5) << 4) | (5))) ) << 8) | (   ((int)(((5) << 4) | (5))) )))))) ;
    m_cclass[23] = ((int)(((  ((int)(((  ((int)((( 5) << 4) | (5)))  ) << 8) | (((int)(((5) << 4) | ( 5))) ))) ) << 16) | (  ((int)(((  ((int)(((5) << 4) | (5))) ) << 8) | (   ((int)(((5) << 4) | (5))) )))))) ;
    m_cclass[24] = ((int)(((  ((int)(((  ((int)((( 6) << 4) | (6)))  ) << 8) | (((int)(((6) << 4) | ( 6))) ))) ) << 16) | (  ((int)(((  ((int)(((6) << 4) | (6))) ) << 8) | (   ((int)(((0) << 4) | (0))) )))))) ;
    m_cclass[25] = ((int)(((  ((int)(((  ((int)((( 6) << 4) | (6)))  ) << 8) | (((int)(((6) << 4) | ( 6))) ))) ) << 16) | (  ((int)(((  ((int)(((6) << 4) | (6))) ) << 8) | (   ((int)(((6) << 4) | (6))) )))))) ;
    m_cclass[26] = ((int)(((  ((int)(((  ((int)((( 6) << 4) | (6)))  ) << 8) | (((int)(((6) << 4) | ( 6))) ))) ) << 16) | (  ((int)(((  ((int)(((6) << 4) | (6))) ) << 8) | (   ((int)(((6) << 4) | (6))) )))))) ;
    m_cclass[27] = ((int)(((  ((int)(((  ((int)((( 6) << 4) | (6)))  ) << 8) | (((int)(((6) << 4) | ( 6))) ))) ) << 16) | (  ((int)(((  ((int)(((6) << 4) | (6))) ) << 8) | (   ((int)(((6) << 4) | (6))) )))))) ;
    m_cclass[28] = ((int)(((  ((int)(((  ((int)((( 8) << 4) | (8)))  ) << 8) | (((int)(((8) << 4) | ( 8))) ))) ) << 16) | (  ((int)(((  ((int)(((8) << 4) | (8))) ) << 8) | (   ((int)(((8) << 4) | (7))) )))))) ;
    m_cclass[29] = ((int)(((  ((int)(((  ((int)((( 8) << 4) | (8)))  ) << 8) | (((int)(((9) << 4) | ( 8))) ))) ) << 16) | (  ((int)(((  ((int)(((8) << 4) | (8))) ) << 8) | (   ((int)(((8) << 4) | (8))) )))))) ;
    m_cclass[30] = ((int)(((  ((int)(((  ((int)((( 11) << 4) | (11)))  ) << 8) | (((int)(((11) << 4) | ( 11))) ))) ) << 16) | (  ((int)(((  ((int)(((11) << 4) | (11))) ) << 8) | (   ((int)(((11) << 4) | (10))) )))))) ;
    m_cclass[31] = ((int)(((  ((int)(((  ((int)((( 0) << 4) | (0)))  ) << 8) | (((int)(((15) << 4) | ( 14))) ))) ) << 16) | (  ((int)(((  ((int)(((13) << 4) | (13))) ) << 8) | (   ((int)(((13) << 4) | (12))) )))))) ;

    m_states = new int[26] ;

    m_states[0] = ((int)(((  ((int)(((  ((int)(((      10) << 4) | (     12)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eStart) << 4) | (eError))) )))))) ;
    m_states[1] = ((int)(((  ((int)(((  ((int)(((      3) << 4) | (     4)))  ) << 8) | (((int)(((     5) << 4) | (      6))) ))) ) << 16) | (  ((int)(((  ((int)(((     7) << 4) | (     8))) ) << 8) | (   ((int)(((     11) << 4) | (     9))) )))))) ;
    m_states[2] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[3] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[4] = ((int)(((  ((int)(((  ((int)((( eItsMe) << 4) | (eItsMe)))  ) << 8) | (((int)(((eItsMe) << 4) | ( eItsMe))) ))) ) << 16) | (  ((int)(((  ((int)(((eItsMe) << 4) | (eItsMe))) ) << 8) | (   ((int)(((eItsMe) << 4) | (eItsMe))) )))))) ;
    m_states[5] = ((int)(((  ((int)(((  ((int)((( eItsMe) << 4) | (eItsMe)))  ) << 8) | (((int)(((eItsMe) << 4) | ( eItsMe))) ))) ) << 16) | (  ((int)(((  ((int)(((eItsMe) << 4) | (eItsMe))) ) << 8) | (   ((int)(((eItsMe) << 4) | (eItsMe))) )))))) ;
    m_states[6] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((     5) << 4) | (      5))) ))) ) << 16) | (  ((int)(((  ((int)(((     5) << 4) | (     5))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[7] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[8] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((     5) << 4) | (      5))) ))) ) << 16) | (  ((int)(((  ((int)(((     5) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[9] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[10] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((     7) << 4) | (      7))) ))) ) << 16) | (  ((int)(((  ((int)(((     7) << 4) | (     7))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[11] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[12] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((     7) << 4) | (      7))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[13] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[14] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((     9) << 4) | (      9))) ))) ) << 16) | (  ((int)(((  ((int)(((     9) << 4) | (     9))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[15] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[16] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((     9) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[17] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[18] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((     12) << 4) | (      12))) ))) ) << 16) | (  ((int)(((  ((int)(((     12) << 4) | (     12))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[19] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[20] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((     12) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[21] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[22] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | (      12))) ))) ) << 16) | (  ((int)(((  ((int)(((     12) << 4) | (     12))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[23] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[24] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eStart) << 4) | ( eStart))) ))) ) << 16) | (  ((int)(((  ((int)(((eStart) << 4) | (eStart))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;
    m_states[25] = ((int)(((  ((int)(((  ((int)((( eError) << 4) | (eError)))  ) << 8) | (((int)(((eError) << 4) | ( eError))) ))) ) << 16) | (  ((int)(((  ((int)(((eError) << 4) | (eError))) ) << 8) | (   ((int)(((eError) << 4) | (eError))) )))))) ;

    m_charset = _T("UTF-8");
    m_stFactor =  16;

}

