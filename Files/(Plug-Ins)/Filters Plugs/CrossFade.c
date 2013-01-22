/*	CrossFade		*/
/*	v 0.3			*/
/*	1995 by Liane	*/

//	Usage:
//	After setting a not-so-bad loop, do loop-to-selection
//	then crossfade. It will remove the click that might
//	appear at the end of the loop, and also linearize
//	the harmonic content in it.

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

OSErr mainCrossFade(void				*unused,
					sData					*theData,
					long					SelectionStart,
					long					SelectionEnd,
					PPInfoPlug				*thePPInfoPlug,
					short					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	switch( theData->amp)
	{
		case 8:
		{
			long	i, j, length = (SelectionEnd - SelectionStart) / 2;
			long	temp, tempStart, tempEnd;
			Ptr		StartPtr, EndPtr;
		
			length = min (length, theData->size - SelectionEnd);
			length = min (length, SelectionStart);
			length *= 2;
			
			for( j = 0; j < 2; j++)	//do it twice
			{
				StartPtr = theData->data + SelectionStart - (length / 2);
				EndPtr = theData->data + SelectionEnd - (length / 2);
				for( i = 0; i < length; i++)
				{
					tempStart = *StartPtr;
					tempEnd = *EndPtr;
					
					temp = ((tempEnd * i) + (tempStart * (length - i))) / length;
					if( temp > 127) temp = 127;
					else if( temp < -127 ) temp = -127;
					*StartPtr++ = temp;
			
					temp = ((tempStart * i) + (tempEnd * (length - i))) / length;
					if( temp > 127) temp = 127;
					else if( temp < -127 ) temp = -127;
					*EndPtr++ = temp;
				}
			}
		} break;

		case 16:
		{
			long	i, j, length = (SelectionEnd - SelectionStart) / 2;
			long	temp, tempStart, tempEnd;
			short	*StartPtr, *EndPtr;
		
			length = min (length, theData->size - SelectionEnd);
			length = min (length, SelectionStart);
//			length *= 2;
			
			for( j = 0; j < 2; j++)	//do it twice
			{
				StartPtr = (short*) theData->data + (SelectionStart / 2) - (length / 2);
				EndPtr = (short*) theData->data + (SelectionEnd / 2) - (length / 2);
				for( i = 0; i < length; i++)
				{
					tempStart = *StartPtr;
					tempEnd = *EndPtr;
			
					temp = ((tempEnd * i) + (tempStart * (length - i))) / length;
					if( temp >= (short)0x7FFF) temp = 0x7FFF;	// overflow ?
					else if( temp <= (short)0x8000 ) temp = (short)0x8000;
					*StartPtr++ = temp;
			
					temp = ((tempStart * i) + (tempEnd * (length - i))) / length;
					if( temp >= (short)0x7FFF) temp = 0x7FFF;	// overflow ?
					else if( temp <= (short)0x8000 ) temp = (short)0x8000;
					*EndPtr++ = temp;
				}
			}
		} break;
	}
	
	return noErr;
}

// C3D9EA39-A386-4636-B115-6B517215F095
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0xC3, 0xD9, 0xEA, 0x39, 0xA3, 0x86, 0x46, 0x36, 0xB1, 0x15, 0x6B, 0x51, 0x72, 0x15, 0xF0, 0x95)

#define PLUGMAIN mainCrossFade
#define PLUGINFACTORY CrossFadeFactory

#include "CFPlugin-FilterBridge.c"

