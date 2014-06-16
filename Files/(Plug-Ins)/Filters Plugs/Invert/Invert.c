/*	Invert			*/
/*	v 1.0			*/
/*	1999 by ROSSET	*/

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>

static OSErr mainInvert(void		*unused,
						sData		*theData,
						long		SelectionStart,
						long		SelectionEnd,
						PPInfoPlug	*thePPInfoPlug,
						short		StereoMode) // StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	int i, temp;
	
	switch (theData->amp) {
		case 8:
		{
			char *SamplePtr = (theData->data) + SelectionStart;
			for (i = 0; i < SelectionEnd - SelectionStart; i++) {
				temp = *SamplePtr;
				temp = 0xFF - temp;
				*SamplePtr++ = temp;
				
				if (StereoMode) {
					SamplePtr++;
					i++;
				}
			}
		}
			break;
			
		case 16:
		{
			short *SamplePtr = (short*) theData->data + (SelectionStart / 2);
			for (i = 0; i < (SelectionEnd - SelectionStart) / 2; i++) {
				temp = (int)*SamplePtr;
				temp = 0xFFFF - temp;
				*SamplePtr++ = temp;
				
				if (StereoMode) {
					SamplePtr++;
					i++;
				}
			}
		}
			break;
	}
	
	return MADNoErr;
}

// E81B53F4-8CEE-4D32-98F2-780ADB447EB1
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xE8, 0x1B, 0x53, 0xF4, 0x8C, 0xEE, 0x4D, 0x32, 0x98, 0xF2, 0x78, 0x0A, 0xDB, 0x44, 0x7E, 0xB1)
#define PLUGMAIN mainInvert
#define PLUGINFACTORY InvertFactory

#include "CFPlugin-FilterBridge.c"
