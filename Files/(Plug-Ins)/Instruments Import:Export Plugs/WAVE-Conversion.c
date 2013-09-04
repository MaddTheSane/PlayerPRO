/*	WAV		*/
/*  IMPORT	*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#include <PlayerPROCore/RDriverInt.h>
#include "WAV.h"

#ifndef __LP64__
#include <QuickTime/QuickTime.h>
#endif

#define WAVE_FORMAT_PCM		1
#define kmaxVolume			7

#ifndef USEDEPRECATEDFUNCS
#define USEDEPRECATEDFUNCS 1
#endif

OSErr TestWAV( PCMWavePtr CC)
{
	if( EndianU32_BtoN(CC->ckid) =='RIFF') return noErr;
	else return MADFileNotSupportedByThisPlug;
}

/*___________________ long byte swap for Intel <-> Motorola Conversions*/
//The following will only byteswap on a Big-endian machine

/*static inline UInt32 longswap(UInt32 ul)
{
	return EndianS32_LtoN(ul);
}*/
#define longswap(ul) EndianU32_LtoN(ul)

/*___________________ word byte swap for Intel <-> Motorola Conversions*/
//The following will only byteswap on a Big-endian machine

/*static inline UInt16 shrtswap(UInt16 us)
{
	return EndianS16_LtoN(us);
}*/
#define shrtswap(us) EndianU16_LtoN(us)

/*_______________________________________________________________________*/


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


Ptr ConvertWAVCFURL(CFURLRef theURL, size_t *sndSize, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *rate, Boolean *stereo)
{
	PCMWavePtr	WAVERsrc = NULL;
	UNFILE		fRef;//, tempResRef, x;
	long		fSize;
	//int			theHit;
	//char		test;
 	//short		gRefNum;
 	//short		gVolSet;

	{
		CFIndex theLen = getCFURLFilePathRepresentationLength(theURL, true);
		char *cfPath = calloc(theLen, 1), *file;
		CFURLGetFileSystemRepresentation(theURL, true, (unsigned char*)cfPath, theLen);
		size_t StrLen = strlen(cfPath);
		file = malloc(++StrLen);
		if (!file) {
			file = cfPath;
		} else {
			strlcpy(file, cfPath, StrLen);
			free(cfPath);
		}
		fRef = iFileOpenRead(file);
		free(file);
	}
	
	
	*stereo = false;
	
	{
		fSize = iGetEOF(fRef);
		if(!(WAVERsrc = (PCMWavePtr) malloc(fSize)))
		{
			iClose(fRef); return NULL;
		}
		
		if(iRead(fSize, (Ptr)WAVERsrc, fRef))
		{
			iClose(fRef); return NULL;
		}
		
		if(EndianU32_BtoN((*WAVERsrc).ckid) =='RIFF')
		{
			(*WAVERsrc).cksize = longswap((*WAVERsrc).cksize);
			
			if(EndianU32_BtoN((*WAVERsrc).fccType) =='WAVE')
			{
				(*WAVERsrc).dwDataOffset = longswap((*WAVERsrc).dwDataOffset);
				
				if(EndianU32_BtoN((*WAVERsrc).fmtType) == 'fmt ')
				{
					(*WAVERsrc).wFormatTag      = shrtswap((*WAVERsrc).wFormatTag);
					(*WAVERsrc).nCannels        = shrtswap((*WAVERsrc).nCannels);
					(*WAVERsrc).nSamplesPerSec  = longswap((*WAVERsrc).nSamplesPerSec);
					(*WAVERsrc).nSamplesPerSec  = (*WAVERsrc).nSamplesPerSec << 16;
					(*WAVERsrc).nAvgBytesPerSec = longswap((*WAVERsrc).nAvgBytesPerSec);
					(*WAVERsrc).nBlockAlign     = shrtswap((*WAVERsrc).nBlockAlign);
					(*WAVERsrc).wBitsPerSample  = shrtswap((*WAVERsrc).wBitsPerSample);
					(*WAVERsrc).dataSize        = longswap((*WAVERsrc).dataSize);
					
					*loopStart	= 0;
					*loopEnd 	= 0;
					*sampleSize = (*WAVERsrc).wBitsPerSample;
					*rate		= (*WAVERsrc).nSamplesPerSec;
					
					if( (*WAVERsrc).nCannels == 2) *stereo = true;
					else *stereo = false;
					
					if((*WAVERsrc).wFormatTag != 1)
					{
						return NULL;
					}
				}
				else
				{
					free( WAVERsrc);
					return NULL;
				}	
			}
			else
			{
				free(WAVERsrc);
				return NULL;
			}
		}
		else
		{
			free(WAVERsrc);
			return NULL;
		}
		iClose(fRef);
	}
	
	{
		*sndSize = WAVERsrc->dataSize;
		
		memmove(WAVERsrc, WAVERsrc->theData, *sndSize);		
		WAVERsrc = realloc( WAVERsrc, *sndSize);
		
		switch( *sampleSize)
		{
			case 8:
				ConvertInstrumentIn( (Byte*) WAVERsrc, *sndSize);
			break;
			
			case 16:
				{
					long			i;
					unsigned short	*tt = (unsigned short*) WAVERsrc;
					
					i = (*sndSize)/2;
					while( i-- > 0) tt[ i] = shrtswap( tt[ i]);
				}
			break;
		}
	}
	return (Ptr) WAVERsrc;
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
	
	tmpPtr = ConvertWAVCFURL(tmpURL, &ptrLen, loopStart, loopEnd, sampleSize, rate, stereo);
	
	//We need this to match old behavior
	ptrReturn = NewPtr(ptrLen);
	memcpy(ptrReturn, tmpPtr, ptrLen);
	free(tmpPtr);
	CFRelease(tmpURL);
	
	return ptrReturn;
}

OSErr ConvertDataToWAVE( FSSpec file, FSSpec *newfile, PPInfoPlug *thePPInfoPlug)
{
	OSErr					iErr;
	Boolean					canceled;
	Movie 					theMovie;
	FSIORefNum				resRefNum, resId;
	Cursor					watchCrsr;
	CursHandle				myCursH;
	Str255					resName;
	Boolean					dataRefWasChanged;
	
	iErr = EnterMovies();
	if( iErr)
	{
		return -1;
	}
	
	myCursH = GetCursor( 357);
	
	if( myCursH == NULL) Debugger();
	DetachResource( (Handle) myCursH);		HLock( (Handle) myCursH);
	watchCrsr = **myCursH;					HUnlock( (Handle) myCursH);		DisposeHandle((Handle) myCursH);
	
	resRefNum = 0;
	iErr = OpenMovieFile ( &file, &resRefNum, fsCurPerm);
	
	resId = 0;
	iErr = NewMovieFromFile( &theMovie, resRefNum, &resId, resName, 0, &dataRefWasChanged);
	
	//CallUpdateALLWindowUPP();
	
	canceled = FALSE;
	
	if( !canceled && iErr == noErr)
	{
		iErr = FindFolder( kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &newfile->vRefNum, &newfile->parID);
		if( iErr == noErr)
		{
			/////////////////////////////////////////////////
			//		WAVE CONVERSION
			/////////////////////////////////////////////////
			
			FSMakeFSSpec(newfile->vRefNum, newfile->parID, file.name, newfile);
			
			SetCursor( &watchCrsr);
			
			iErr = FSpDelete( newfile);
			
			iErr = ConvertMovieToFile(	theMovie,
										0,
							 			newfile,
							 			'WAVE',
							 			'SNPL',
							 			smCurrentScript,
							 			NULL,
							 			0,
							 			0);
			
			DisposeMovie( theMovie);
			
			CloseMovieFile( resRefNum);
		}
	}
	
	ExitMovies();
	
	return iErr;
}
#endif
