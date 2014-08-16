/*	WAV		*/
/*  IMPORT	*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
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

OSErr TestWAV(PCMWavePtr CC)
{
	if (CFSwapInt32BigToHost(CC->ckid) =='RIFF')
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


static CFIndex getCFURLFilePathRepresentationLength(CFURLRef theRef, Boolean resolveAgainstBase)
{
	CFStringRef fileString = CFURLCopyFileSystemPath(theRef, kCFURLPOSIXPathStyle);
	CFIndex strLength = CFStringGetMaximumSizeOfFileSystemRepresentation(fileString);
	CFRelease(fileString);
	
	return strLength;
}


void *ConvertWAVCFURL(CFURLRef theURL, size_t *sndSize, long *loopStart, long *loopEnd, short *sampleSize, unsigned int *rate, Boolean *stereo)
{
	PCMWavePtr	WAVERsrc = NULL;
	UNFILE		fRef;
	long		fSize;
	
	{
		CFIndex theLen = getCFURLFilePathRepresentationLength(theURL, true);
		char *cfPath = alloca(theLen);
		CFURLGetFileSystemRepresentation(theURL, true, (unsigned char*)cfPath, theLen);
		fRef = iFileOpenRead(cfPath);
	}
	
	*stereo = false;
	
	if (fRef != NULL) {
		fSize = iGetEOF(fRef);
		if (!(WAVERsrc = (PCMWavePtr)malloc(fSize))) {
			iClose(fRef);
			return NULL;
		}
		
		if (iRead(fSize, WAVERsrc, fRef)) {
			free(WAVERsrc);
			iClose(fRef);
			return NULL;
		}
		
		if (CFSwapInt32BigToHost(WAVERsrc->ckid) =='RIFF') {
			WAVERsrc->cksize = longswap(WAVERsrc->cksize);
			
			if (CFSwapInt32BigToHost(WAVERsrc->fccType) =='WAVE') {
				WAVERsrc->dwDataOffset = longswap(WAVERsrc->dwDataOffset);
				
				if (CFSwapInt32BigToHost(WAVERsrc->fmtType) == 'fmt ') {
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
						iClose(fRef);
						return NULL;
					}
				} else {
					free(WAVERsrc);
					iClose(fRef);
					return NULL;
				}
			} else {
				free(WAVERsrc);
				iClose(fRef);
				return NULL;
			}
		} else {
			free(WAVERsrc);
			iClose(fRef);
			return NULL;
		}
		iClose(fRef);
	}
	
	{
		unsigned short *tt;
		
		*sndSize = WAVERsrc->dataSize;
		memmove(WAVERsrc, WAVERsrc->theData, *sndSize);
		WAVERsrc = realloc(WAVERsrc, *sndSize);
		
		switch (*sampleSize) {
			case 8:
				ConvertInstrumentIn((Byte*)WAVERsrc, *sndSize);
				break;
				
			case 16:
				tt = (unsigned short*)WAVERsrc;
				dispatch_apply((*sndSize) / 2, dispatch_get_global_queue(0, 0), ^(size_t i) {
					tt[i] = shrtswap(tt[i]);
				});
				break;
		}
	}
	return WAVERsrc;
}

#if !defined(__LP64__) && USEDEPRECATEDFUNCS
#ifdef QD_HEADERS_ARE_PRIVATE
//Workaround so it can build on 10.7 and later SDKs
typedef struct Cursor {
	short data[16];
	short mask[16];
	Point hotSpot;
} Cursor, *CursPtr, **CursHandle;
extern CursHandle GetCursor(short) DEPRECATED_ATTRIBUTE;
extern void SetCursor(const Cursor *) DEPRECATED_ATTRIBUTE;
#endif

Ptr ConvertWAV(FSSpec *fileSpec, long *loopStart, long *loopEnd, short	*sampleSize, unsigned long *rate, Boolean *stereo)
{
	Ptr ptrReturn = NULL;
	Ptr tmpPtr = NULL;
	FSRef tmpRef;
	CFURLRef tmpURL;
	
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
	OSErr		iErr;
	Boolean		canceled;
	Movie 		theMovie;
	FSIORefNum	resRefNum, resId;
	Cursor		watchCrsr;
	CursHandle	myCursH;
	Str255		resName;
	Boolean		dataRefWasChanged;
	
	iErr = EnterMovies();
	if (iErr)
		return -1;
	
	myCursH = GetCursor(357);
	
	if (myCursH == NULL) Debugger();
	DetachResource((Handle)myCursH);
	HLock((Handle)myCursH);
	watchCrsr = **myCursH;
	HUnlock((Handle)myCursH);
	DisposeHandle((Handle)myCursH);
	
	resRefNum = 0;
	iErr = OpenMovieFile(&file, &resRefNum, fsCurPerm);
	
	resId = 0;
	iErr = NewMovieFromFile(&theMovie, resRefNum, &resId, resName, 0, &dataRefWasChanged);
	
	//CallUpdateALLWindowUPP();
	
	canceled = FALSE;
	
	if (!canceled && iErr == MADNoErr) {
		iErr = FindFolder(kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &newfile->vRefNum, &newfile->parID);
		if (iErr == MADNoErr) {
			/////////////////////////////////////////////////
			//		WAVE CONVERSION
			/////////////////////////////////////////////////
			
			FSMakeFSSpec(newfile->vRefNum, newfile->parID, file.name, newfile);
			
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
