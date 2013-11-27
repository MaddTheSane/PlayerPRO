/*	Smooth			*/
/*	v 0.2			*/
/*	1995 by Liane	*/

//	Usage:
//	Works like a low pass filter, removing high
//	harmonics generated by a low sampling rate.
//	Works on the selected part or all the waveform
//	if there is no selection.
//	Not very accurate, but pretty fast to write !!!

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>

static OSErr mainSmooth(void			*unused,
						sData			*theData,
						long			SelectionStart,
						long			SelectionEnd,
						PPInfoPlug		*thePPInfoPlug,
						short			StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long	i, length, temp, prevtemp, nexttemp, work;

	length = SelectionEnd - SelectionStart - 1;

	switch( theData->amp)
	{
		case 8:
		{
			Ptr	SamplePtr = (theData->data) + SelectionStart;
			
			prevtemp = *SamplePtr++;
			temp = *SamplePtr++;
			for (i = 1; i < length; i++)
			{
				nexttemp = *SamplePtr--;
				
				work = ((prevtemp + nexttemp) + (temp * 6)) >> 3;
				
				*SamplePtr++ = work;
				prevtemp = temp;
				temp = nexttemp;
				SamplePtr++;
			}
		} break;

		case 16:
		{
			short	*SamplePtr = (short*) theData->data + (SelectionStart / 2);
			
			prevtemp = *SamplePtr++;
			temp = *SamplePtr++;
			for (i = 1; i < length / 2; i++)
			{
				nexttemp = *SamplePtr--;
				
				work = ((prevtemp + nexttemp) + (temp * 6)) >> 3;
				
				*SamplePtr++ = work;
				prevtemp = temp;
				temp = nexttemp;
				SamplePtr++;
			}
		} break;
	}

	return noErr;
}

// CC5BCE43-E362-4260-BB51-555A5CD4008C
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xCC, 0x5B, 0xCE, 0x43, 0xE3, 0x62, 0x42, 0x60, 0xBB, 0x51, 0x55, 0x5A, 0x5C, 0xD4, 0x00, 0x8C)
#define PLUGMAIN mainSmooth
#define PLUGINFACTORY SmoothFactory

#include "CFPlugin-FilterBridge.c"
