#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>
#include "Amplitude.h"

OSErr mainAmplitude(void					*unused,
					sData					*theData,
					long					SelectionStart,
					long					SelectionEnd,
					PPInfoPlug				*thePPInfoPlug,
					short					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	SInt32	i, temp, Inc;
	Ptr		Sample8Ptr = theData->data;
	short	*Sample16Ptr = (short*) theData->data;

	Inc = 120;
	if( getParams( &Inc, thePPInfoPlug))
	{
		switch( theData->amp)
		{
			case 8:
				Sample8Ptr += SelectionStart;
				
				for( i = 0; i < SelectionEnd - SelectionStart; i++)
				{
					temp = *Sample8Ptr;
					if( temp >= 0x80) temp -= 0xFF;
						
					temp *= Inc;
					temp /= 100L;
					if( temp >= 127) temp = 127;
					else if( temp <= -127 ) temp = -127;
						
					*Sample8Ptr = temp;
					
					if( StereoMode)
					{
						Sample8Ptr++;
						i++;
					}
					
					Sample8Ptr++;
				}
			break;
			
			case 16:
				Sample16Ptr += SelectionStart/2;						// Div 2, because it's in bytes !!!
			
				for( i = 0; i < (SelectionEnd - SelectionStart)/2; i++)	// Div 2, because it's in bytes !!!
				{
					temp = *Sample16Ptr;
					
					temp *= Inc;
					temp /= 100L;
					
					if( temp >= (short) 0x7FFF) temp = 0x7FFF;	// overflow ?
					else if( temp <= (short) 0x8000 ) temp = (short) 0x8000;
					
					*Sample16Ptr = temp;
					
					if( StereoMode)
					{
						Sample16Ptr++;
						i++;
					}
					
					Sample16Ptr++;
				}
			break;
		}
	}
	return noErr;
}

#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xF1, 0xE5, 0xEA, 0x0B, 0x1A, 0xBC, 0x4D, 0x5F, 0x89, 0xDF, 0x3C, 0x89, 0x75, 0x8E, 0xAC, 0x0E))
//F1E5EA0B-1ABC-4D5F-89DF-3C89758EAC0E

#define PLUGMAIN mainAmplitude
#define PLUGINFACTORY AmplitudeFactory

#include "CFPlugin-FilterBridge.c"
