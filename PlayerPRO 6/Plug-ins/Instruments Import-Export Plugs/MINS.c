/*	MINS			*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>

static inline OSErr TestMINS(InstrData *CC)
{
	if (CC->type == 0 && CC->numSamples >= 0 && CC->numSamples < MAXSAMPLE)
		return MADNoErr;
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
	memset(curIns, 0, sizeof(InstrData));
	curIns->firstSample = firstSample;
	
	return MADNoErr;
}

static inline void ByteswapsData(sData *toswap)
{
	MADBE32(&toswap->size);
	MADBE16(&toswap->c2spd);
	MADBE32(&toswap->loopBeg);
	MADBE32(&toswap->loopSize);
}

static inline void ByteswapInstrument(InstrData *toswap)
{
	MADBE16(&toswap->MIDIType);
	MADBE16(&toswap->MIDI);
	MADBE16(&toswap->firstSample);
	MADBE16(&toswap->numSamples);
	MADBE16(&toswap->volFade);
	
	dispatch_apply(12, dispatch_get_global_queue(0, 0), ^(size_t i) {
		MADBE16(&toswap->pannEnv[i].pos);
		MADBE16(&toswap->pannEnv[i].val);
		MADBE16(&toswap->pitchEnv[i].pos);
		MADBE16(&toswap->pitchEnv[i].val);
		MADBE16(&toswap->volEnv[i].pos);
		MADBE16(&toswap->volEnv[i].val);
	});
}

static OSErr mainMINs(void			*unused,
					  OSType		order,						// Order to execute
					  InstrData		*InsHeader,					// Ptr on instrument header
					  sData			**sample,					// Ptr on samples data
					  short			*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
					  // If sampleID == -1 : add sample else replace selected sample.
					  CFURLRef		AlienFileCFURL,				// IN/OUT file
					  PPInfoPlug	*thePPInfoPlug)
{
	OSErr				myErr = MADNoErr;
	short				x;
	long				inOutCount;
	UInt8*				theSound;
	CFStreamStatus		theStatus;
	CFReadStreamRef		readStream;
	CFWriteStreamRef	writeStream;
	
	switch (order) {
		case MADPlugImport:
			readStream = CFReadStreamCreateWithFile(kCFAllocatorDefault, AlienFileCFURL);
			if (CFReadStreamOpen(readStream)) {
				theStatus = CFReadStreamGetStatus(readStream);
				while (theStatus != kCFStreamStatusOpen && theStatus != kCFStreamStatusError) {
					theStatus = CFReadStreamGetStatus(readStream);
				}
				if (theStatus == kCFStreamStatusError) {
					CFReadStreamClose(readStream);
					CFRelease(readStream);
					return MADReadingErr;
				}
				MAD2KillInstrument(InsHeader, sample);
				
				// READ instrument header
				
				inOutCount = sizeof(InstrData);
				CFReadStreamRead(readStream, (UInt8*)InsHeader, inOutCount);
				ByteswapInstrument(InsHeader);
				
				// READ samples headers & data
				
				for (x = 0; x < InsHeader->numSamples; x++) {
					sData *curData = sample[x] = MADCreateSampleRaw();
					
					inOutCount = sizeof(sData32);
					CFReadStreamRead(readStream, (UInt8*)curData, inOutCount);
					ByteswapsData(curData);
					curData->data = malloc(curData->size);
					if (curData->data != NULL) {
						inOutCount = curData->size;
						CFReadStreamRead(readStream, (UInt8*)curData->data, inOutCount);
					}
					if (curData->amp == 16) {
						short	*shortPtr = (short*)curData->data;
						dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t ll) {
							MADBE16(&shortPtr[ll]);
						});
					}
				}
				
				CFReadStreamClose(readStream);
			} else
				myErr = MADReadingErr;
			CFRelease(readStream);
			break;
			
		case MADPlugTest:
			readStream = CFReadStreamCreateWithFile(kCFAllocatorDefault, AlienFileCFURL);
			if (CFReadStreamOpen(readStream)) {
				theStatus = CFReadStreamGetStatus(readStream);
				while (theStatus != kCFStreamStatusOpen && theStatus != kCFStreamStatusError) {
					theStatus = CFReadStreamGetStatus(readStream);
				}
				if (theStatus == kCFStreamStatusError) {
					CFReadStreamClose(readStream);
					CFRelease(readStream);
					return MADReadingErr;
				}
				inOutCount = sizeof(InstrData);
				theSound = malloc(inOutCount);
				if (theSound == NULL) {
					CFReadStreamClose(readStream);
					CFRelease(readStream);
					return MADNeedMemory;
				} else {
					CFReadStreamRead(readStream, theSound, inOutCount);
					ByteswapInstrument((InstrData*)theSound);
					myErr = TestMINS((InstrData*)theSound);
					free(theSound);
					theSound = NULL;
				}
				
				CFReadStreamClose(readStream);
			} else
				myErr = MADReadingErr;
			CFRelease(readStream);
			break;
			
		case MADPlugExport:
			writeStream = CFWriteStreamCreateWithFile(kCFAllocatorDefault, AlienFileCFURL);
			if (CFWriteStreamOpen(writeStream)) {
				theStatus = CFWriteStreamGetStatus(writeStream);
				while (theStatus != kCFStreamStatusOpen && theStatus != kCFStreamStatusError) {
					theStatus = CFWriteStreamGetStatus(writeStream);
				}
				if (theStatus == kCFStreamStatusError) {
					CFWriteStreamClose(writeStream);
					CFRelease(writeStream);
					return MADWritingErr;
				}
				inOutCount = sizeof(InstrData);
				InstrData *tempIns = malloc(inOutCount);
				memcpy(tempIns, InsHeader, inOutCount);
				ByteswapInstrument(tempIns);
				CFWriteStreamWrite(writeStream, (UInt8*)tempIns, inOutCount);
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
							MADBE16(&shortPtr[ll]);
						});
					}
					
					memcpy(&toWrite, copyData, sizeof(sData32));
					toWrite.data = 0;
					
					inOutCount = sizeof(sData32);
					CFWriteStreamWrite(writeStream, (UInt8*)&toWrite, inOutCount);
					
					inOutCount = curData->size;
					CFWriteStreamWrite(writeStream, (UInt8*)copydataData, inOutCount);
					free(copyData);
					free(copydataData);
				}
				CFWriteStreamClose(writeStream);
			} else
				myErr = MADWritingErr;
			CFRelease(writeStream);
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}

// 9C897935-C00B-4AAC-81D6-E43049E3A8E0
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x9C, 0x89, 0x79, 0x35, 0xC0, 0x0B, 0x4A, 0xAC, 0x81, 0xD6, 0xE4, 0x30, 0x49, 0xE3, 0xA8, 0xE0)
#define PLUGINFACTORY MINsFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainMINs //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"
