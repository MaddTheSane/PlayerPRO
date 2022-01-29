/*	v 1.0			*/
/*	1999 by ANR		*/

#include "PPPlug.h"
#include "AIFF.h"
#include "soundinput.h"
#include "QuicktimeComponents.h"
#include "Sound.h"
#include "Movies.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE( sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( OSType)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( InstrData*)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof( sData**)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof( short*)))
		| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof( FSSpec*)))
		| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof( PPInfoPlug*)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif

OSErr main(		OSType					order,						// Order to execute
				InstrData				*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				FSSpec					*AlienFileFSSpec,			// IN/OUT file
				PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	short	iFileRefI;
	long	inOutBytes;
	
	#ifndef powerc
		long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
	#endif
	
	switch( order)
	{
		case 'PLAY':
		break;
		
		case 'IMPL':
		{
			Ptr				theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			FSSpec			newFile;
			
			myErr = ConvertDataToWAVE( *AlienFileFSSpec, &newFile, thePPInfoPlug);
			if( myErr == noErr)
			{
				theSound = ConvertWAV( &newFile, &lS, &lE, &sS, &rate, &stereo);
				
				if( theSound) inAddSoundToMAD( theSound, lS, lE, sS, 60, rate, stereo, newFile.name, InsHeader, sample, sampleID);
				else myErr = MADNeedMemory;
				
				FSpDelete( &newFile);
			}
		}
		break;
		
		case 'TEST':
		{
			FInfo fInfo;
			
			FSpGetFInfo( AlienFileFSSpec, &fInfo);
			
			if( fInfo.fdType == kQTFileTypeAVI) myErr = noErr;
			else myErr = MADFileNotSupportedByThisPlug;
		}
		break;
		
		case 'EXPL':
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
	#ifndef powerc
		SetA4( oldA4);
	#endif
	
	return myErr;
}