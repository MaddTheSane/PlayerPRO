/*	Backwards		*/
/*	v 0.2			*/
/*	1995 by Liane	*/

//	Usage:
//	Invert the selected part or all the waveform if
//	there is no selection.

#include <PlayerPROCore/PlayerPROCore.h>

static OSErr mainBackwards(void			*unused,
						   sData		*theData,
						   long			SelectionStart,
						   long			SelectionEnd,
						   PPInfoPlug	*thePPInfoPlug,
						   short		StereoMode) // StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	size_t			i;
	unsigned short	temp1, temp2;
	
	switch (theData->amp) {
		case 8:
		{
			char *orgPtr = theData->data, *destPtr = orgPtr;

			orgPtr += SelectionStart;
			destPtr += SelectionEnd - 1;
			
			for (i = 0; i < (SelectionEnd - SelectionStart) / 2; i++) {	//just swap values
				temp1 = *orgPtr;
				temp2 = *destPtr;
				
				*orgPtr++ = temp2;
				*destPtr-- = temp1;
				
				if (StereoMode) {
					orgPtr++;
					destPtr--;
					i++;
				}
			}
		}
			break;
		
		case 16:
		{
			unsigned short	*orgPtr = (unsigned short*) theData->data, *destPtr = orgPtr;

			orgPtr += SelectionStart / 2;
			destPtr += (SelectionEnd - 1) / 2;
			
			for (i = 0; i < (SelectionEnd - SelectionStart) / 4; i++) {
				temp1 = *orgPtr;
				temp2 = *destPtr;
				*orgPtr++ = temp2;
				*destPtr-- = temp1;
				
				if (StereoMode) {
					orgPtr++;
					destPtr--;
					i++;
				}
			}
		}
			break;
	}

	return MADNoErr;
}

#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xAF, 0x64, 0x04, 0x51, 0x90, 0xE8, 0x45, 0x11, 0x83, 0x7B, 0x58, 0xA8, 0xF3, 0x50, 0x24, 0xF0))
//AF640451-90E8-4511-837B-58A8F35024F0
#define PLUGMAIN mainBackwards
#define PLUGINFACTORY BackwardsFactory

#include "CFPlugin-FilterBridge.c"
