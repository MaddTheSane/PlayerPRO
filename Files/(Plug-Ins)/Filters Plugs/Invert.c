/*	Invert			*/
/*	v 1.0			*/
/*	1999 by ROSSET	*/

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