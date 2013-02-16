/*	Normalize		*/
/*	v 0.2			*/
/*	1995 by Liane	*/

//	Usage:
//	Another version of "Maximize", but this one
//	will try to get the maximum gain.
//	Works on the selected part or all the waveform
//	if there is no selection.

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define labs(a) (((a) < 0) ? (-a) : (a))

static OSErr mainNormalize(void					*unused,
						   sData				*theData,
						   long					SelectionStart,
						   long					SelectionEnd,
						   PPInfoPlug			*thePPInfoPlug,
						   short				StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long	i, peak = 0, temp;

	switch( theData->amp)
	{
		case 8:
		{
			Ptr	SamplePtr = (theData->data) + SelectionStart;
			for( i = 0; i < SelectionEnd - SelectionStart; i++)
			{
				temp = *SamplePtr++;
				
				peak = max (peak, labs(temp));
				
				if( StereoMode)
				{
					SamplePtr++;
					i++;
				}
			}
			
			if( peak != 0)
			{
				peak = ((long)0x80 * 0x10000) / peak;
				
				SamplePtr = (theData->data) + SelectionStart;
				for( i = 0; i < SelectionEnd - SelectionStart; i++)
				{
					temp = *SamplePtr;
					
					temp = (peak * temp) / 0x10000;
			
					if( temp > 127) temp = 127;
					else if( temp < -127 ) temp = -127;
					
					*SamplePtr++ = temp;
					
					if( StereoMode)
					{
						SamplePtr++;
						i++;
					}
				}
			}
		} break;

		case 16:
		{
			short	*SamplePtr = (short*) theData->data + (SelectionStart / 2);

			for( i = 0; i < (SelectionEnd - SelectionStart) / 2; i++)
			{
				temp = (long)*SamplePtr++;
				peak = max (peak, labs(temp));
				
				if( StereoMode)
				{
					SamplePtr++;
					i++;
				}
			}
			
			if( peak != 0)
			{
				peak = ((long)0x8000 * 0x10000) / peak;
				
				SamplePtr = (short*) theData->data + (SelectionStart / 2);
				for( i = 0; i < (SelectionEnd - SelectionStart) / 2; i++)
				{
					temp = (long)*SamplePtr;
			
					temp = -(peak * temp) / 0x10000;
			
					*SamplePtr++ = temp;
					
					if( StereoMode)
					{
						SamplePtr++;
						i++;
					}
				}
			}
		} break;
	}
	
	return noErr;
}

// 9AAC8E6C-DAD4-453A-8A90-17D27CF2FFBC
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x9A, 0xAC, 0x8E, 0x6C, 0xDA, 0xD4, 0x45, 0x3A, 0x8A, 0x90, 0x17, 0xD2, 0x7C, 0xF2, 0xFF, 0xBC)

#define PLUGMAIN mainNormalize
#define PLUGINFACTORY NormalizeFactory

#include "CFPlugin-FilterBridge.c"
