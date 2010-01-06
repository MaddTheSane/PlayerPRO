/*	Invert			*/
/*	v 1.0			*/
/*	1999 by ROSSET	*/

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>

OSErr mainInvert(	sData					*theData,
					long					SelectionStart,
					long					SelectionEnd,
					PPInfoPlug				*thePPInfoPlug,
					short					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long	i, peak = 0, temp;

	switch( theData->amp)
	{
		case 8:
		{
			Ptr	SamplePtr = (theData->data) + SelectionStart;
			for( i = 0; i < SelectionEnd - SelectionStart; i++)
			{
				temp = *SamplePtr;
				
				temp = 0xFF - temp;
				
				*SamplePtr++ = temp;
				
				if( StereoMode)
				{
					SamplePtr++;
					i++;
				}
			}
		} break;

		case 16:
		{
			short	*SamplePtr = (short*) theData->data + (SelectionStart / 2);

			for( i = 0; i < (SelectionEnd - SelectionStart) / 2; i++)
			{
				temp = (long)*SamplePtr;
				
				temp = 0xFFFF - temp;
				
				*SamplePtr++ = temp;
				
				if( StereoMode)
				{
					SamplePtr++;
					i++;
				}
			}
		} break;
	}
	
	return noErr;
}

// E81B53F4-8CEE-4D32-98F2-780ADB447EB1
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0xE8, 0x1B, 0x53, 0xF4, 0x8C, 0xEE, 0x4D, 0x32, 0x98, 0xF2, 0x78, 0x0A, 0xDB, 0x44, 0x7E, 0xB1)

#define PLUGMAIN mainInvert
#define PLUGINFACTORY InvertFactory

#include "CFPlugin-FilterBridge.c"
