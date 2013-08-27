//
//  ToneGenerator.h
//  PPMacho
//
//  Created by C.W. Betts on 8/12/13.
//
//

#ifndef PPMacho_ToneGenerator_h
#define PPMacho_ToneGenerator_h

#include <PlayerPROCore/PlayerPROCore.h>

extern char *Audio8Ptr __attribute__((visibility ("hidden")));
extern short *Audio16Ptr __attribute__((visibility ("hidden")));

typedef enum _ToneGenerator
{
	cancelTone,
	silence = 3,
	triangle = 4,
	square = 5,
	wave = 6
} ToneGenerator;

__private_extern__ int RunToneGeneratorPlug(sData *theData, PPInfoPlug *thePPInfoPlug, long *audioLen, int stereoMode, long *audioAmp, long *audioFreq);
__private_extern__ short* CreateAudio16Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo);
__private_extern__ Ptr CreateAudio8Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo);

#endif
