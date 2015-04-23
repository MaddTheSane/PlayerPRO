/*	WAV		*/
/*  IMPORT	*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/RDriverInt.h>
#include <CoreFoundation/CFByteOrder.h>
#include "WAV.h"

//TODO: re-write this to use the AudioToolbox OS X framework: it can natively handle wave audio.

#ifndef __LP64__
#include <QuickTime/QuickTime.h>
#endif


enum {
	WAVE_FORMAT_PCM = 1,
	kmaxVolume = 7
};

#ifndef USEDEPRECATEDFUNCS
#define USEDEPRECATEDFUNCS 1
#endif

MADErr TestWAV(const PCMWavePtr CC)
{
	if (memcmp(CC->ckid, "RIFF", 4) == 0)
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

/*___________________ long byte swap for Intel <-> Motorola Conversions*/
//The following will only byteswap on a Big-endian machine

#define longswap(ul) CFSwapInt32LittleToHost(ul)

/*___________________ word byte swap for Intel <-> Motorola Conversions*/
//The following will only byteswap on a Big-endian machine

#define shrtswap(us) CFSwapInt16LittleToHost(us)

/*_______________________________________________________________________*/


static CFIndex URLGetFileSize(CFURLRef theURL)
{
	CFNumberRef fileSizeRef = NULL;
	CFIndex fileSize = 200;
	if (!CFURLCopyResourcePropertyForKey(theURL, kCFURLFileSizeKey, &fileSizeRef, NULL)) {
		return fileSize;
	}
	
	fileSize = CFNumberGetValue(fileSizeRef, kCFNumberCFIndexType, &fileSize);
	CFRelease(fileSizeRef);
	return fileSize;
}

void *ConvertWAVCFURL(CFURLRef theURL, size_t *sndSize, int *loopStart, int *loopEnd, short *sampleSize, unsigned int *rate, bool *stereo)
{
	PCMWavePtr	WAVERsrc = NULL;
	long		fSize = 0;
	CFReadStreamRef fRef = CFReadStreamCreateWithFile(kCFAllocatorDefault, theURL);
	CFReadStreamOpen(fRef);
	CFStreamStatus theStat = CFReadStreamGetStatus(fRef);
	while (theStat != kCFStreamStatusOpen && theStat != kCFStreamStatusError) {
		theStat = CFReadStreamGetStatus(fRef);
	}
	if (theStat == kCFStreamStatusError) {
		CFReadStreamClose(fRef);
		CFRelease(fRef);
		return NULL;
	}
	
	*stereo = false;
	
	if (fRef != NULL) {
		fSize = URLGetFileSize(theURL);
		if (!(WAVERsrc = (PCMWavePtr)malloc(fSize))) {
			CFReadStreamClose(fRef);
			CFRelease(fRef);
			return NULL;
		}
		
		if (CFReadStreamRead(fRef, (UInt8*)WAVERsrc, fSize) != fSize) {
			free(WAVERsrc);
			CFReadStreamClose(fRef);
			CFRelease(fRef);
			return NULL;
		}
		
		if (memcmp(WAVERsrc->ckid, "RIFF", 4) == 0) {
			WAVERsrc->cksize = longswap(WAVERsrc->cksize);
			
			if (memcmp(WAVERsrc->fccType, "WAVE", 4) == 0) {
				WAVERsrc->dwDataOffset = longswap(WAVERsrc->dwDataOffset);
				
				if (memcmp(WAVERsrc->fmtType, "fmt ", 4) == 0) {
					WAVERsrc->wFormatTag		= shrtswap(WAVERsrc->wFormatTag);
					WAVERsrc->nCannels			= shrtswap(WAVERsrc->nCannels);
					WAVERsrc->nSamplesPerSec	= longswap(WAVERsrc->nSamplesPerSec);
					WAVERsrc->nSamplesPerSec	= CFSwapInt32BigToHost(WAVERsrc->nSamplesPerSec) << 16; //FIXME: is this right for LE machines?
					WAVERsrc->nAvgBytesPerSec	= longswap(WAVERsrc->nAvgBytesPerSec);
					WAVERsrc->nBlockAlign		= shrtswap(WAVERsrc->nBlockAlign);
					WAVERsrc->wBitsPerSample	= shrtswap(WAVERsrc->wBitsPerSample);
					WAVERsrc->dataSize			= longswap(WAVERsrc->dataSize);
					
					*loopStart	= 0;
					*loopEnd 	= 0;
					*sampleSize = WAVERsrc->wBitsPerSample;
					*rate		= WAVERsrc->nSamplesPerSec;
					
					if (WAVERsrc->nCannels == 2)
						*stereo = true;
					else
						*stereo = false;
					
					if (WAVERsrc->wFormatTag != 1) {
						free(WAVERsrc);
						CFReadStreamClose(fRef);
						CFRelease(fRef);
						return NULL;
					}
				} else {
					free(WAVERsrc);
					CFReadStreamClose(fRef);
					CFRelease(fRef);
					return NULL;
				}
			} else {
				free(WAVERsrc);
				CFReadStreamClose(fRef);
				CFRelease(fRef);
				return NULL;
			}
		} else {
			free(WAVERsrc);
			CFReadStreamClose(fRef);
			CFRelease(fRef);
			return NULL;
		}
		CFReadStreamClose(fRef);
		CFRelease(fRef);
	}
	
	{
#if __BIG_ENDIAN__
		unsigned short *tt;
#endif
		
		*sndSize = WAVERsrc->dataSize;
		memmove(WAVERsrc, WAVERsrc->theData, *sndSize);
		WAVERsrc = realloc(WAVERsrc, *sndSize);
		
		switch (*sampleSize) {
			case 8:
				MADConvertInstrument((Byte*)WAVERsrc, *sndSize);
				break;
				
			case 16:
#if __BIG_ENDIAN__
				tt = (unsigned short*)WAVERsrc;
				dispatch_apply((*sndSize) / 2, dispatch_get_global_queue(0, 0), ^(size_t i) {
					tt[i] = shrtswap(tt[i]);
				});
#endif
				break;
		}
	}
	return WAVERsrc;
}

#if !defined(__LP64__) && USEDEPRECATEDFUNCS
#ifdef QD_HEADERS_ARE_PRIVATE

#pragma pack(push, 2)
//Workaround so it can build on 10.7 and later SDKs
typedef struct Cursor {
	short data[16];
	short mask[16];
	Point hotSpot;
} Cursor, *CursPtr, **CursHandle;
extern CursHandle GetCursor(short) DEPRECATED_ATTRIBUTE;
extern void SetCursor(const Cursor *) DEPRECATED_ATTRIBUTE;
#pragma pack(pop)
#endif

Ptr ConvertWAV(FSSpec *fileSpec, int *loopStart, int *loopEnd, short *sampleSize, unsigned long *rate, bool *stereo)
{
	Ptr ptrReturn = NULL;
	Ptr tmpPtr = NULL;
	FSRef tmpRef = {0};
	CFURLRef tmpURL = NULL;
	
	FSpMakeFSRef(fileSpec, &tmpRef);
	tmpURL = CFURLCreateFromFSRef(kCFAllocatorDefault, &tmpRef);
	size_t ptrLen = 0;
	unsigned int tmpVal = *rate;
	tmpPtr = ConvertWAVCFURL(tmpURL, &ptrLen, loopStart, loopEnd, sampleSize, &tmpVal, stereo);
	*rate = tmpVal;
	CFRelease(tmpURL);
	//We need this to match old behavior
	ptrReturn = NewPtr(ptrLen);
	memcpy(ptrReturn, tmpPtr, ptrLen);
	free(tmpPtr);
	
	return ptrReturn;
}

OSErr ConvertDataToWAVE(FSSpec file, FSSpec *newfile, PPInfoPlug *thePPInfoPlug)
{
	OSErr		iErr = noErr;
	Boolean		canceled = false;
	Movie 		theMovie = NULL;
	FSIORefNum	resRefNum = 0, resId = 0;
	Cursor		watchCrsr = {0};
	CursHandle	myCursH = NULL;
	Str255		resName = {0};
	Boolean		dataRefWasChanged = false;
	
	iErr = EnterMovies();
	if (iErr)
		return -1;
	
	myCursH = GetCursor(357);
	
	if (myCursH == NULL) {
		MADDebugStr(__LINE__, __FILE__, "");
		return MADUnknownErr;
	}
	DetachResource((Handle)myCursH);
	HLock((Handle)myCursH);
	watchCrsr = **myCursH;
	HUnlock((Handle)myCursH);
	DisposeHandle((Handle)myCursH);
	
	iErr = OpenMovieFile(&file, &resRefNum, fsCurPerm);
	if (iErr) {
		ExitMovies();
		return -1;
	}
	
	iErr = NewMovieFromFile(&theMovie, resRefNum, &resId, resName, 0, &dataRefWasChanged);
	
	//CallUpdateALLWindowUPP();
	
	canceled = FALSE;
	
	if (!canceled && iErr == noErr) {
		iErr = FindFolder(kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &newfile->vRefNum, &newfile->parID);
		if (iErr == noErr) {
			/////////////////////////////////////////////////
			//		WAVE CONVERSION
			/////////////////////////////////////////////////
			
			iErr = FSMakeFSSpec(newfile->vRefNum, newfile->parID, file.name, newfile);
			
			SetCursor(&watchCrsr);
			
			iErr = FSpDelete(newfile);
			
			iErr = ConvertMovieToFile(theMovie, 0, newfile, 'WAVE', 'SNPL', smCurrentScript, NULL, 0, 0);
			
			DisposeMovie(theMovie);
			CloseMovieFile(resRefNum);
		}
	}
	
	ExitMovies();
	
	return iErr;
}
#endif
