//
//  PP6Detector.h
//  PPMacho
//
//  Created by C.W. Betts on 8/12/14.
//
//

#ifndef __PPMacho__PP6Detector__
#define __PPMacho__PP6Detector__

#include <stdbool.h>

static inline bool IsPlayerPRO6()
{
#if defined(PLAYERPRO6) && PLAYERPRO6
	return true;
#else
	return false;
#endif
}

#endif /* defined(__PPMacho__PP6Detector__) */
