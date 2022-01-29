/*	Backwards		*/
/*	v 0.2			*/
/*	1995 by Liane	*/

//	Usage:
//	Invert the selected part or all the waveform if
//	there is no selection.

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


OSErr main( 	sData					*theData,
				long					SelectionStart,
				long					SelectionEnd,
				PPInfoPlug				*thePPInfoPlug,
				long					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long	i;
	unsigned short	temp1, temp2;
	
	switch( theData->amp)
	{
		case 8:
		{
			Ptr	orgPtr = theData->data, destPtr = orgPtr;

			orgPtr += SelectionStart;
			destPtr += SelectionEnd - 1;
			
			for( i = 0; i < (SelectionEnd - SelectionStart) / 2; i++)	//just swap values
			{
				temp1 = *orgPtr;
				temp2 = *destPtr;
				
				*orgPtr++ = temp2;
				*destPtr-- = temp1;
				
				if( StereoMode)
				{
					orgPtr++;
					destPtr--;
					i++;
				}
			}
		} break;
		
		case 16:
		{
			unsigned short	*orgPtr = (unsigned short*) theData->data, *destPtr = orgPtr;

			orgPtr += SelectionStart / 2;
			destPtr += (SelectionEnd - 1) / 2;
			
			for( i = 0; i < (SelectionEnd - SelectionStart) / 4; i++)
			{
				temp1 = *orgPtr;
				temp2 = *destPtr;
				*orgPtr++ = temp2;
				*destPtr-- = temp1;
				
				if( StereoMode)
				{
					orgPtr++;
					destPtr--;
					i++;
				}
			}
		} break;
	}

	return noErr;
}
