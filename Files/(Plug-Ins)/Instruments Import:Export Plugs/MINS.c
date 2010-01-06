/*	MINS			*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <Sound.h>

static OSErr TestMINS( InstrData *CC)
{
	if( CC->type == 0 && CC->numSamples >= 0 && CC->numSamples < MAXSAMPLE) return noErr;
	else return MADFileNotSupportedByThisPlug;
}

static OSErr MAD2KillInstrument( InstrData *curIns, sData **sample)
{
	short		i;
//	Boolean		IsReading;

	for( i = 0; i < curIns->numSamples; i++)
	{
		if( sample[ i] != NULL)
		{
			if( sample[ i]->data != NULL)
			{
				DisposePtr( (Ptr) sample[ i]->data);
				sample[ i]->data = NULL;
			}
			DisposePtr( (Ptr) sample[ i]);
			sample[ i] = NULL;
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

OSErr mainMINs(	OSType					order,						// Order to execute
				InstrData				*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				FSSpec					*AlienFileFSSpec,			// IN/OUT file
				PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr;
	UNFILE	iFileRefI;
	short	x;
	long	inOutCount;
	Ptr		theSound;

	switch( order)
	{
		case 'IMPL':
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				GetEOF( iFileRefI, &inOutCount);
				
				theSound = NewPtr( inOutCount);
				if( theSound == NULL) myErr = MADNeedMemory;
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
						if( curData->data != NULL)
						{
							inOutCount = curData->size;
							myErr = FSRead( iFileRefI, &inOutCount, curData->data);
						}
					}
				}
				
				FSCloseFork( iFileRefI);
			}
		
			break;
		
		case 'TEST':
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				inOutCount = 50L;
				theSound = NewPtr( inOutCount);
				if( theSound == NULL) myErr = MADNeedMemory;
				else
				{
					FSRead( iFileRefI, &inOutCount, theSound);
					
					myErr = TestMINS( (InstrData*) theSound);
				}
				
				DisposePtr( theSound);
				
				FSCloseFork( iFileRefI);
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
				FSCloseFork( iFileRefI);
			}
			break;
		
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}

// 9C897935-C00B-4AAC-81D6-E43049E3A8E0
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x9C, 0x89, 0x79, 0x35, 0xC0, 0x0B, 0x4A, 0xAC, 0x81, 0xD6, 0xE4, 0x30, 0x49, 0xE3, 0xA8, 0xE0)
#define PLUGINFACTORY MINsFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainMINs //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"
