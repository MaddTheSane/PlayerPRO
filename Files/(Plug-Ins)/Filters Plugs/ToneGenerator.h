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

extern char *Audio8Ptr;
extern short *Audio16Ptr;

typedef enum _ToneGenerator
{
	cancelTone,
	silence = 3,
	triangle = 4,
	square = 5,
	wave = 6
} ToneGenerator;

extern int RunToneGeneratorPlug(sData *theData, PPInfoPlug *thePPInfoPlug, long *audioLen, int stereoMode, long *audioAmp, long *audioFreq);
extern short* CreateAudio16Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo);
extern Ptr CreateAudio8Ptr( long AudioLength, long AudioFreq, long AudioAmp, long AudioType, Boolean stereo);

#endif
