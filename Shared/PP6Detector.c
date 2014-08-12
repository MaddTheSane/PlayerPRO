//
//  PP6Detector.c
//  PPMacho
//
//  Created by C.W. Betts on 8/12/14.
//
//

//#include "PP6Detector.h"

bool IsPlayerPRO6()
{
#if defined(PLAYERPRO6) && PLAYERPRO6
	return true;
#else
	return false;
#endif
}
