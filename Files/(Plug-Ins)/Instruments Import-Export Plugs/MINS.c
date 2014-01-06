/*	MINS			*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

static OSErr TestMINS(InstrData *CC)
{
	if (CC->type == 0 && CC->numSamples >= 0 && CC->numSamples < MAXSAMPLE)
		return noErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static OSErr MAD2KillInstrument(InstrData *curIns, sData **sample)
{
	short firstSample;
	
	if (!curIns || !sample)
		return MADParametersErr;
	
	dispatch_apply(curIns->numSamples, dispatch_get_global_queue(0, 0), ^(size_t i) {
		if (sample[i] != NULL) {
			if (sample[i]->data != NULL) {
				free(sample[i]->data);
				sample[i]->data = NULL;
			}
			free(sample[i]);
			sample[i] = NULL;
		}
	});
	
	firstSample = curIns->firstSample;
	
	memset(curIns, 0, sizeof(*curIns));
	curIns->firstSample = firstSample;
	
#if 0
	for (i = 0; i < 32; i++) curIns->name[i]	= 0;
	curIns->type		= 0;
	curIns->numSamples	= 0;
	
	/**/
	
#if 1
	memset(curIns->what, 0, sizeof(curIns->what));
	memset(curIns->volEnv, 0, sizeof(curIns->volEnv));
	memset(curIns->pannEnv, 0, sizeof(curIns->pannEnv));
	memset(curIns->pitchEnv, 0, sizeof(curIns->pitchEnv));
#else
	for (i = 0; i < 96; i++) curIns->what[ i]		= 0;
	
	for (i = 0; i < 12; i++)
	{
		curIns->volEnv[ i].pos		= 0;
		curIns->volEnv[ i].val		= 0;
		
		curIns->pannEnv[ i].pos	= 0;
		curIns->pannEnv[ i].val	= 0;
		
		curIns->pitchEnv[ i].pos	= 0;
		curIns->pitchEnv[ i].val	= 0;
		
	}
#endif
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
#endif
	return noErr;
}

static inline void ByteswapsData(sData *toswap)
{
	PPBE32(&toswap->size);
	PPBE16(&toswap->c2spd);
	PPBE32(&toswap->loopBeg);
	PPBE32(&toswap->loopSize);
}

static inline void ByteswapInstrument(InstrData *toswap)
{
	PPBE16(&toswap->MIDIType);
	PPBE16(&toswap->MIDI);
	PPBE16(&toswap->firstSample);
	PPBE16(&toswap->numSamples);
	PPBE16(&toswap->volFade);
	
	dispatch_apply(12, dispatch_get_global_queue(0, 0), ^(size_t i) {
		PPBE16(&toswap->pannEnv[i].pos);
		PPBE16(&toswap->pannEnv[i].val);
		
		PPBE16(&toswap->pitchEnv[i].pos);
		PPBE16(&toswap->pitchEnv[i].val);
		
		PPBE16(&toswap->volEnv[i].pos);
		PPBE16(&toswap->volEnv[i].val);
	});
}

//hack around the fact that there isn't an equivalent of CFStringGetMaximumSizeOfFileSystemRepresentation for CFURLs
static CFIndex getCFURLFilePathRepresentationLength(CFURLRef theRef, Boolean resolveAgainstBase)
{
	CFStringRef fileString = CFURLCopyFileSystemPath(theRef, kCFURLPOSIXPathStyle);
	CFIndex strLength = CFStringGetMaximumSizeOfFileSystemRepresentation(fileString);
	CFRelease(fileString);
	return strLength;
}

static OSErr mainMINs(void		*unused,
			   OSType		order,						// Order to execute
			   InstrData	*InsHeader,					// Ptr on instrument header
			   sData		**sample,					// Ptr on samples data
			   short		*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
			   // If sampleID == -1 : add sample else replace selected sample.
			   CFURLRef		AlienFileCFURL,				// IN/OUT file
			   PPInfoPlug	*thePPInfoPlug)
{
	OSErr	myErr = noErr;
	UNFILE	iFileRefI;
	short	x;
	long	inOutCount;
	Ptr		theSound;
	
	char *file = NULL;
	do {
		char *longStr = NULL;
		CFIndex pathLen = getCFURLFilePathRepresentationLength(AlienFileCFURL, TRUE);
		longStr = malloc(pathLen);
		if (!longStr) {
			return MADNeedMemory;
		}
		Boolean pathOK = CFURLGetFileSystemRepresentation(AlienFileCFURL, true, (unsigned char*)longStr, pathLen);
		if (!pathOK) {
			free(longStr);
			return MADReadingErr;
		}
		size_t StrLen = strlen(longStr);
		file = realloc(longStr, ++StrLen);
		if (!file) {
			file = longStr;
		}
	} while (0);
	
	switch (order)
	{
		case 'IMPL':
			iFileRefI = iFileOpenRead(file);
			if (iFileRefI != NULL) {
				inOutCount = iGetEOF(iFileRefI);
				
				theSound = malloc(inOutCount);
				if (theSound == NULL)
					myErr = MADNeedMemory;
				else {
					free(theSound);
					
					MAD2KillInstrument(InsHeader, sample);
					
					// READ instrument header
					
					inOutCount = sizeof(InstrData);
					
					iRead(inOutCount, InsHeader, iFileRefI);
					ByteswapInstrument(InsHeader);
					
					// READ samples headers & data
					
					for (x = 0; x < InsHeader->numSamples; x++) {
						sData *curData = sample[x] = inMADCreateSample();
						
						inOutCount = sizeof(sData32);
						
						myErr = iRead(inOutCount, curData, iFileRefI);
						
						ByteswapsData(curData);
						
						curData->data = malloc(curData->size);
						if (curData->data != NULL) {
							inOutCount = curData->size;
							myErr = iRead(inOutCount, (Ptr)curData->data, iFileRefI);
						}
						
						if (curData->amp == 16) {
							short	*shortPtr = (short*)curData->data;
							
							dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t ll) {
								PPBE16(&shortPtr[ll]);
							});
						}
						
					}
				}
				
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			
			break;
			
		case 'TEST':
			iFileRefI = iFileOpenRead(file);
			if (iFileRefI != NULL) {
				inOutCount = sizeof(InstrData);
				theSound = malloc(inOutCount);
				if (theSound == NULL)
					myErr = MADNeedMemory;
				else {
					iRead(inOutCount, theSound, iFileRefI);
					
					ByteswapInstrument((InstrData*)theSound);
					
					myErr = TestMINS((InstrData*)theSound);
				}
				
				free(theSound);
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
			
			break;
			
		case 'EXPL':
			
			iFileCreate(file, 'MINs');
			iFileRefI = iFileOpenWrite(file);
			
			if (iFileRefI != NULL) {
				// Write instrument header
				inOutCount = sizeof(InstrData);
				InstrData *tempIns = malloc(inOutCount);
				memcpy(tempIns, InsHeader, inOutCount);
				ByteswapInstrument(tempIns);
				iWrite(inOutCount, tempIns, iFileRefI);
				free(tempIns);
				
				// Write samples headers & data
				for (x = 0; x < InsHeader->numSamples; x++) {
					sData	*curData;
					sData	*copyData;
					sData32	toWrite;
					Ptr		copydataData;
					curData = sample[x];
					copyData = malloc(sizeof(sData));
					memcpy(copyData, curData, sizeof(sData));
					ByteswapsData(copyData);
					copydataData = malloc(curData->size);
					memcpy(copydataData, curData->data, curData->size);
					if (curData->amp == 16) {
						short	*shortPtr = (short*) copydataData;
						
						dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t ll) {
							PPBE16(&shortPtr[ll]);
							
						});
						
					}
					
					memcpy(&toWrite, copyData, sizeof(sData32));
					toWrite.data = 0;
					
					inOutCount = sizeof(sData32);
					iWrite(inOutCount, &toWrite, iFileRefI);
					
					inOutCount = curData->size;
					iWrite(inOutCount, copydataData, iFileRefI);
					free(copyData);
					free(copydataData);
				}
				iClose(iFileRefI);
			} else
				myErr = MADWritingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	free(file);
	
	return myErr;
}

// 9C897935-C00B-4AAC-81D6-E43049E3A8E0
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x9C, 0x89, 0x79, 0x35, 0xC0, 0x0B, 0x4A, 0xAC, 0x81, 0xD6, 0xE4, 0x30, 0x49, 0xE3, 0xA8, 0xE0)
#define PLUGINFACTORY MINsFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainMINs //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"
