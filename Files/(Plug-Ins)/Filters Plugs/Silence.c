/*
	Player PRO 5.0x PlugIns

	Antoine ROSSET
	16 Tranchees
	1206 GENEVA
	SWITZERLAND
	
	FAX: 022 789 35 03
	Compuserve: 100277,164

/********************************************************/

// Exemple: SILENCE PlugIns. Set Selection to 0.

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

OSErr main( 	sData					*theData,				// Sample Informations
				long					SelectionStart,			// SelectionStart in bytes ! Even for 16bits audio and stereo
				long					SelectionEnd,			// SelectionEnd in bytes ! Even for 16bits audio and stereo
				PPInfoPlug				*thePPInfoPlug,
				long					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long	i;
	Ptr		Sample8Ptr = theData->data;
	short	*Sample16Ptr = (short*) theData->data;
	
	if( Sample8Ptr == 0L) return noErr;
	
	switch( theData->amp)
	{
		case 8:
			Sample8Ptr += SelectionStart;
			
			for( i = 0; i < SelectionEnd - SelectionStart; i++)
			{
				*Sample8Ptr = 0;
				
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
				*Sample16Ptr = 0;
				
				if( StereoMode)
				{
					Sample16Ptr++;
					i++;
				}
				
				Sample16Ptr++;
			}
		break;
	}
	
	return noErr;
}