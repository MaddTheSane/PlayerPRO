/*
	Player PRO 5.0x PlugIns

	Antoine ROSSET
	16 Tranchees
	1206 GENEVA
	SWITZERLAND
	
	FAX: 022 789 35 03
	Compuserve: 100277,164
 */

/********************************************************/

// Exemple: SILENCE PlugIns. Set Selection to 0.

#include <PlayerPROCore/PlayerPROCore.h>

static OSErr mainSilence(	sData			*theData,				// Sample Informations
					long			SelectionStart,			// SelectionStart in bytes ! Even for 16bits audio and stereo
					long			SelectionEnd,			// SelectionEnd in bytes ! Even for 16bits audio and stereo
					PPInfoPlug		*thePPInfoPlug,
					short			StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long	i;
	Ptr		Sample8Ptr = theData->data;
	short	*Sample16Ptr = (short*) theData->data;
	
	if (Sample8Ptr == NULL) return noErr;
	
	switch (theData->amp)
	{
		case 8:
			Sample8Ptr += SelectionStart;
			
			for (i = 0; i < SelectionEnd - SelectionStart; i++)
			{
				*Sample8Ptr = 0;
				
				if (StereoMode)
				{
					Sample8Ptr++;
					i++;
				}
				
				Sample8Ptr++;
			}
		break;
		
		case 16:
			Sample16Ptr += SelectionStart/2;						// Div 2, because it's in bytes !!!
			
			for (i = 0; i < (SelectionEnd - SelectionStart)/2; i++)	// Div 2, because it's in bytes !!!
			{
				*Sample16Ptr = 0;
				
				if (StereoMode)
				{
					Sample16Ptr++;
					i++;
				}
				
				Sample16Ptr++;
			}
		break;
	}
	
	return noErr;
}

// 54DEED74-6B67-4B3F-8B31-68283410C1D7
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x54, 0xDE, 0xED, 0x74, 0x6B, 0x67, 0x4B, 0x3F, 0x8B, 0x31, 0x68, 0x28, 0x34, 0x10, 0xC1, 0xD7)

#define PLUGMAIN mainSilence
#define PLUGINFACTORY SilenceFactory

#include "CFPlugin-FilterBridge.c"
