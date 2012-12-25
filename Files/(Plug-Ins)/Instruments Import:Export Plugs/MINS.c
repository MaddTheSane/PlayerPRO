/*	MINS			*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

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
				free( sample[ i]->data);
				sample[ i]->data = NULL;
			}
			free( sample[ i]);
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

//hack around the fact that there isn't an equivalent of CFStringGetMaximumSizeOfFileSystemRepresentation for CFURLs
static CFIndex getCFURLFilePathRepresentationLength(CFURLRef theRef, Boolean resolveAgainstBase)
{
	CFURLRef toDeref = theRef;
	if (resolveAgainstBase) {
		toDeref = CFURLCopyAbsoluteURL(theRef);
	}
	CFStringRef fileString = CFURLCopyFileSystemPath(toDeref, kCFURLPOSIXPathStyle);
	CFIndex strLength = CFStringGetMaximumSizeOfFileSystemRepresentation(fileString);
	CFRelease(fileString);
	if (resolveAgainstBase) {
		CFRelease(toDeref);
	}
	return strLength;
}


OSErr mainMINs(	OSType					order,						// Order to execute
				InstrData				*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				CFURLRef					AlienFileCFURL,			// IN/OUT file
				PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr;
	UNFILE	iFileRefI;
	short	x;
	long	inOutCount;
	Ptr		theSound;
	
	char *file = NULL;
	{
		CFIndex pathLen = getCFURLFilePathRepresentationLength(AlienFileCFURL, TRUE);
		file = malloc(pathLen);
		if (!file) {
			return MADNeedMemory;
		}
		Boolean pathOK = CFURLGetFileSystemRepresentation(AlienFileCFURL, true, (unsigned char*)file, pathLen);
		if (!pathOK) {
			free(file);
			return MADReadingErr;
		}
		
	}


	switch( order)
	{
		case 'IMPL':
			iFileRefI = iFileOpenRead(file);
			if( iFileRefI != NULL)
			{
				inOutCount = iGetEOF(iFileRefI);
				
				theSound = malloc( inOutCount);
				if( theSound == NULL) myErr = MADNeedMemory;
				else
				{
					free( theSound);
					
					MAD2KillInstrument( InsHeader, sample);
					
					// READ instrument header
					
					inOutCount = sizeof( InstrData);
					
					iRead(inOutCount, (Ptr)InsHeader, iFileRefI);
					
					// READ samples headers & data
					
					for( x = 0; x < InsHeader->numSamples; x++)
					{
						sData *curData = sample[ x] = inMADCreateSample();
						
						inOutCount = sizeof( sData32);
						
						myErr = iRead(inOutCount, (Ptr)curData, iFileRefI);
						
						curData->data = malloc( curData->size);
						if( curData->data != NULL)
						{
							inOutCount = curData->size;
							myErr = iRead(inOutCount, (Ptr)curData->data, iFileRefI);
						}
					}
				}
				
				iClose(iFileRefI);
			}
		
			break;
		
		case 'TEST':
			iFileRefI = iFileOpenRead(file);
			if( iFileRefI != NULL)
			{
				inOutCount = 50L;
				theSound = malloc( inOutCount);
				if( theSound == NULL) myErr = MADNeedMemory;
				else
				{
					iRead(inOutCount, theSound, iFileRefI);
					
					myErr = TestMINS( (InstrData*) theSound);
				}
				
				free( theSound);
				
				iClose(iFileRefI);
			}
		
			break;
		
		case 'EXPL':
			
			iFileCreate(file, 'MINs');
			iFileRefI = iFileOpenWrite(file);
			
			if( myErr == noErr)
			{
				// Write instrument header
				
				inOutCount = sizeof( InstrData);
				iWrite(inOutCount, (Ptr)InsHeader, iFileRefI);
				
				// Write samples headers & data
				
				for( x = 0; x < InsHeader->numSamples; x++)
				{
					sData	*curData;
					
					curData = sample[ x];
					
					inOutCount = sizeof( sData32);
					iWrite(inOutCount, (Ptr)curData, iFileRefI);
					
					inOutCount = curData->size;
					iWrite(inOutCount, curData->data, iFileRefI);

				}
				iClose( iFileRefI);
			}
			break;
		
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	free(file);
	
	return myErr;
}

// 9C897935-C00B-4AAC-81D6-E43049E3A8E0
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x9C, 0x89, 0x79, 0x35, 0xC0, 0x0B, 0x4A, 0xAC, 0x81, 0xD6, 0xE4, 0x30, 0x49, 0xE3, 0xA8, 0xE0)
#define PLUGINFACTORY MINsFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainMINs //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"
