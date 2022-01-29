/*	MINS			*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include "PPPlug.h"
#include "sound.h"

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

OSErr TestMINS( InstrData *CC)
{
	if( CC->type == 0 && CC->numSamples >= 0 && CC->numSamples < MAXSAMPLE) return noErr;
	else return MADFileNotSupportedByThisPlug;
}

OSErr MAD2KillInstrument( InstrData *curIns, sData **sample)
{
short			i;
Boolean			IsReading;

	for( i = 0; i < curIns->numSamples; i++)
	{
		if( sample[ i] != 0L)
		{
			if( sample[ i]->data != 0L)
			{
				DisposePtr( (Ptr) sample[ i]->data);
				sample[ i]->data = 0L;
			}
			DisposePtr( (Ptr) sample[ i]);
			sample[ i] = 0L;
		}
	}
	
	
	for( i = 0; i < 32; i++) curIns->name[ i]	= 0;
	curIns->type		= 0;
	curIns->numSamples	= 0;
	
	/**/
	
	for( i = 0; i < 96; i++) curIns->what[ i]		= 0;
	for( i = 0; i < 12; i++)
	{
		curIns->volEnv[ i].pos		= 0;
		curIns->volEnv[ i].val		= 0;
	}
	for( i = 0; i < 12; i++)
	{
		curIns->pannEnv[ i].pos	= 0;
		curIns->pannEnv[ i].val	= 0;
	}
	for( i = 0; i < 12; i++)
	{
		curIns->pitchEnv[ i].pos	= 0;
		curIns->pitchEnv[ i].val	= 0;
	}
	curIns->volSize		= 0;
	curIns->pannSize	= 0;
	
	curIns->volSus		= 0;
	curIns->volBeg		= 0;
	curIns->volEnd		= 0;
	
	curIns->pannSus		= 0;
	curIns->pannBeg		= 0;
	curIns->pannEnd		= 0;

	curIns->volType		= 0;
	curIns->pannType	= 0;
	
	curIns->volFade		= 0;
	curIns->vibDepth	= 0;
	curIns->vibRate		= 0;
	
	return noErr;
}

OSErr main(		OSType					order,						// Order to execute
				InstrData				*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				FSSpec					*AlienFileFSSpec,			// IN/OUT file
				PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr;
	short	iFileRefI, x;
	long	inOutCount;
	
	#ifndef powerc
		long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
	#endif
	
	switch( order)
	{
		case 'IMPL':
		{
			Ptr				theSound;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				GetEOF( iFileRefI, &inOutCount);
				
				theSound = NewPtr( inOutCount);
				if( theSound == 0L) myErr = MADNeedMemory;
				else
				{
					DisposePtr( theSound);
					
					MAD2KillInstrument( InsHeader, sample);
					
					// READ instrument header
					
					inOutCount = sizeof( InstrData);
					
					myErr = FSRead( iFileRefI, &inOutCount, InsHeader);
					
					// READ samples headers & data
					
					for( x = 0; x < InsHeader->numSamples; x++)
					{
						sData *curData = sample[ x] = inMADCreateSample();
						
						inOutCount = sizeof( sData);
						
						myErr = FSRead( iFileRefI, &inOutCount, curData);
						
						curData->data = NewPtr( curData->size);
						if( curData->data != 0L)
						{
							inOutCount = curData->size;
							myErr = FSRead( iFileRefI, &inOutCount, curData->data);
						}
					}
				}
				
				FSClose( iFileRefI);
			}
		}
		break;
		
		case 'TEST':
		{
			Ptr	theSound;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				inOutCount = 50L;
				theSound = NewPtr( inOutCount);
				if( theSound == 0L) myErr = MADNeedMemory;
				else
				{
					FSRead( iFileRefI, &inOutCount, theSound);
					
					myErr = TestMINS( (InstrData*) theSound);
				}
				
				DisposePtr( theSound);
				
				FSClose( iFileRefI);
			}
		}
		break;
		
		case 'EXPL':
			
			myErr = FSpCreate( AlienFileFSSpec, 'SNPL', 'MINs', smCurrentScript);
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			
			if( myErr == noErr)
			{
				// Write instrument header
				
				inOutCount = sizeof( InstrData);
				myErr = FSWrite( iFileRefI, &inOutCount, InsHeader);
				
				// Write samples headers & data
				
				for( x = 0; x < InsHeader->numSamples; x++)
				{
					sData	*curData;
					
					curData = sample[ x];
					
					inOutCount = sizeof( sData);
					myErr = FSWrite( iFileRefI, &inOutCount, curData);
					
					inOutCount = curData->size;
					myErr = FSWrite( iFileRefI, &inOutCount, curData->data);
				}
				FSClose( iFileRefI);
			}
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
