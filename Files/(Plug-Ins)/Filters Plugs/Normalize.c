/*	Normalize		*/
/*	v 0.2			*/
/*	1995 by Liane	*/

//	Usage:
//	Another version of "Maximize", but this one
//	will try to get the maximum gain.
//	Works on the selected part or all the waveform
//	if there is no selection.

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


#define max(a,b) (((a) > (b)) ? (a) : (b))
#define labs(a) (((a) < 0) ? (-a) : (a))

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