/*	PAT				*/
/*  IMPORT			*/
/*	v 1.0			*/
/*	1999 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#include "PAT.h"

static inline OSErr TestPAT(char *CC)
{
	char	IDStr[12] = "GF1PATCH110";
	short	i;
	
	for (i = 0; i < 12; i++) {
		if (CC[i] != IDStr[i])
			return MADFileNotSupportedByThisPlug;
	}
	
	return MADNoErr;
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

static OSErr PATImport(InstrData *InsHeader, sData **sample, Ptr PATData)
{
	PatchHeader		*PATHeader;
	PatInsHeader	*PATIns;
	//LayerHeader		*PATLayer;
	PatSampHeader	*PATSamp;
	int				i, x;
	unsigned int	scale_table[200] = {
		16351, 17323, 18354, 19445, 20601, 21826, 23124, 24499, 25956, 27500, 29135, 30867,
		32703, 34647, 36708, 38890, 41203, 43653, 46249, 48999, 51913, 54999, 58270, 61735,
		65406, 69295, 73416, 77781, 82406, 87306, 92498, 97998, 103826, 109999, 116540, 123470,
		130812, 138591, 146832, 155563, 164813, 174614, 184997, 195997, 207652, 219999, 233081, 246941,
		261625, 277182, 293664, 311126, 329627, 349228, 369994, 391995, 415304, 440000, 466163, 493883,
		523251, 554365, 587329, 622254, 659255, 698456, 739989, 783991, 830609, 880000, 932328, 987767,
		1046503, 1108731, 1174660, 1244509, 1318511, 1396914, 1479979, 1567983, 1661220, 1760002, 1864657, 1975536,
		2093007, 2217464, 2349321, 2489019, 2637024, 2793830, 2959960, 3135968, 3322443, 3520006, 3729316, 3951073,
		4186073, 4434930, 4698645, 4978041, 5274051, 5587663, 5919922, 6271939, 6644889, 7040015, 7458636, 7902150};
	
	
	// PATCH HEADER
	PATHeader = (PatchHeader*)PATData;
	PATData += 129;
	
	if (PATHeader->InsNo != 1)
		return MADFileNotSupportedByThisPlug;
	
	InsHeader->numSamples = (PATHeader->LoSamp << 8) + PATHeader->HiSamp;
	
	// INS HEADER -- Read only the first instrument
	PATIns = (PatInsHeader*)PATData;
	
	PPLE32(&PATIns->size);
	PATData += 63;
	
	strlcpy(InsHeader->name, PATIns->name, sizeof(PATIns->name));
	
	// LAYERS
	for (i = 0; i < PATIns->layer; i++)
		PATData += 47;
	
	// SAMPLES
	for (x = 0; x < InsHeader->numSamples; x++) {
		sData	*curData;
		Boolean	signedData;
		
		PATSamp = (PatSampHeader*)PATData;
		curData = sample[x] = inMADCreateSample();
		
		strlcpy(curData->name, PATSamp->name, sizeof(PATSamp->name));
		
		PPLE32(&PATSamp->size);
		curData->size		= PATSamp->size;
		PPLE32(&PATSamp->startLoop);
		curData->loopBeg 	= PATSamp->startLoop;
		PPLE32(&PATSamp->endLoop);
		curData->loopSize 	= PATSamp->endLoop - PATSamp->startLoop;
		PPLE16(&PATSamp->rate);
		curData->c2spd		= PATSamp->rate;
		
		
		curData->vol = 64;
		curData->loopType = 0;
		
		if (PATSamp->Flag & 0x01)
			curData->amp = 16;
		else
			curData->amp = 8;
		
		if (PATSamp->Flag & 0x02)
			signedData = true;
		else
			signedData = false;
		
		if (!(PATSamp->Flag & 0x04)) {
			curData->loopBeg = 0;
			curData->loopSize = 0;
		}
		
		if (PATSamp->Flag & 0x08)
			curData->loopType = ePingPongLoop;
		else
			curData->loopType = eClassicLoop;
		
		///////////////
		
		PPLE32(&PATSamp->minFreq);
		PPLE32(&PATSamp->maxFreq);
		PPLE32(&PATSamp->originRate);
		
		for (i = 0; i < 107; i++) {
			if (scale_table[i] >= PATSamp->originRate) {
				PATSamp->originRate = i;
				i = 108;
			}
		}
		
		curData->relNote = 60 - (12 + PATSamp->originRate);
		
		for (i = 0; i < 107; i++) {
			if (scale_table[i] >= PATSamp->minFreq) {
				PATSamp->minFreq = i;
				i = 108;
			}
		}
		
		for (i = 0; i < 107; i++) {
			if (scale_table[i] >= PATSamp->maxFreq) {
				PATSamp->maxFreq = i;
				i = 108;
			}
		}
		
		for (i = PATSamp->minFreq; i < PATSamp->maxFreq; i++) {
			if (i < 96 && i >= 0) {
				InsHeader->what[i] = x;
			}
		}
		
		PATData += 96;
		
		
		// DATA
		curData->data = malloc(curData->size);
		if (curData->data != NULL) {
			memcpy(curData->data, PATData, curData->size);
			
			if (curData->amp == 16) {
				short *tt = (short*) curData->data;
				
				dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t tL) {
					PPLE16((Ptr)(tt + tL));
					
					if (signedData)
						*(tt + tL) += 0x8000;
				});
			} else {
				if (signedData) {
					dispatch_apply(curData->size, dispatch_get_global_queue(0, 0), ^(size_t ixi) {
						curData->data[i] += 0x80;
					});
				}
			}
		}
		
		PATData += PATSamp->size;
	}
	
	return MADNoErr;
}

//hack around the fact that there isn't an equivalent of CFStringGetMaximumSizeOfFileSystemRepresentation for CFURLs
static CFIndex getCFURLFilePathRepresentationLength(CFURLRef theRef, Boolean resolveAgainstBase)
{
	CFStringRef fileString = CFURLCopyFileSystemPath(theRef, kCFURLPOSIXPathStyle);
	CFIndex strLength = CFStringGetMaximumSizeOfFileSystemRepresentation(fileString);
	CFRelease(fileString);
	return strLength;
}


static OSErr mainPAT(void			*unused,
			  OSType		order,						// Order to execute
			  InstrData		*InsHeader,					// Ptr on instrument header
			  sData			**sample,					// Ptr on samples data
			  short			*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
			  // If sampleID == -1 : add sample else replace selected sample.
			  CFURLRef		AlienFileCFURL,			// IN/OUT file
			  PPInfoPlug	*thePPInfoPlug)
{
	OSErr	myErr = MADNoErr;
	UNFILE	iFileRefI;
	long	inOutCount;
	char *file = NULL;
	
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
	file = realloc(longStr, strlen(longStr) + 1);
	if (!file)
		file = longStr;
	
	switch (order) {
		case MADPlugImport:
		{
			Ptr				theSound;
			
			iFileRefI = iFileOpenRead(file);
			if (iFileRefI != NULL) {
				inOutCount = iGetEOF(iFileRefI);
				
				theSound = malloc(inOutCount);
				if (theSound == NULL)
					myErr = MADNeedMemory;
				else {
					iRead(inOutCount, theSound, iFileRefI);
					
					MAD2KillInstrument(InsHeader, sample);
					
					myErr = PATImport(InsHeader, sample, theSound);
					
					free(theSound);
				}
				
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
		}
			break;
			
		case MADPlugTest:
		{
			Ptr	theSound;
			
			iFileRefI = iFileOpenRead(file);
			if (myErr == MADNoErr) {
				inOutCount = 50L;
				theSound = malloc(inOutCount);
				if (theSound == NULL)
					myErr = MADNeedMemory;
				else {
					iRead(inOutCount, theSound, iFileRefI);
					
					myErr = TestPAT(theSound);
				}
				
				free(theSound);
				iClose(iFileRefI);
			} else
				myErr = MADIncompatibleFile;
		}
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	if (file)
		free(file);
	
	
	return myErr;
}

// D54EE3CC-B94C-4245-9E82-2F1D65C0009D
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xD5, 0x4E, 0xE3, 0xCC, 0xB9, 0x4C, 0x42, 0x45, 0x9E, 0x82, 0x2F, 0x1D, 0x65, 0xC0, 0x00, 0x9D)
#define PLUGINFACTORY PATFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainPAT //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"
