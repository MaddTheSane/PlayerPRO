/*	CrossFade		*/
/*	v 0.3			*/
/*	1995 by Liane	*/

//	Usage:
//	After setting a not-so-bad loop, do loop-to-selection
//	then crossfade. It will remove the click that might
//	appear at the end of the loop, and also linearize
//	the harmonic content in it.

#include "MAD.h"
#include "PPPlug.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE( sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( sData*)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( long)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof( long)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof( PPInfoPlug*)))
		| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof( long)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif


#define min(a,b) (((a) < (b)) ? (a) : (b))

OSErr main( 	sData					*theData,
				long					SelectionStart,
				long					SelectionEnd,
				PPInfoPlug				*thePPInfoPlug,
				long					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
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