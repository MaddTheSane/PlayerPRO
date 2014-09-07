/*
 *  WAV.h
 *  PPMacho
 *
 *  Created by C.W. Betts on 12/27/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *  This header file contains the structures that are used by wave files.
 *	They have been condensed into this file so they are the same across the files.
 *
 */

#ifndef __PP_WAV_H_
#define __PP_WAV_H_

#include <PlayerPROCore/MAD.h>
#include <CoreServices/CoreServices.h>

#define __private_extern __attribute__((visibility("hidden")))

#ifndef WIN32
//Windows Defines
#ifndef _INC_DLSMAC
typedef UInt32			ULONG;
typedef SInt32			LONG;
typedef UInt32			DWORD;
typedef UInt16			USHORT;
typedef SInt16			SHORT;
typedef SInt8			BYTE;
#endif
typedef SInt16			WORD;
typedef UInt16			UWORD;
typedef SInt32			HRESULT;
typedef void*			LPVOID;

typedef UInt16			UINT;
#if !defined(BOOL) || !defined(__OBJC__)
//typedef Boolean			BOOL;
#endif
typedef FourCharCode	FOURCC;
#endif

#pragma pack(push, 2)
typedef struct _infoformat_tag {
	Str255	name;
	Str32	smpls;
	Str32	dataBits;
	Str32	stereo;
} InfoRec, *InfoPtr, **InfoHnd;

typedef struct _MMCKINFO {
	FOURCC		ckid;
	DWORD		cksize;
	FOURCC		fccType;
	DWORD		dwDataOffset;
	WORD		dwFlags;
} MMCKINFO;

typedef struct waveformat_tag {
	WORD		wFormatTag;
	WORD		nCannels;
	DWORD		nSamplesPerSec;
	DWORD		nAvgBytesPerSec;
	WORD		nBlockAlign;
	WORD		wBitsPerSample;
} WAVEFORMAT;

typedef struct _pcwaveformat_tag {
	FOURCC		ckid;
	DWORD		cksize;
	FOURCC		fccType;
	FOURCC		fmtType;
	DWORD		dwDataOffset;
	WORD		wFormatTag;
	WORD		nCannels;
	DWORD		nSamplesPerSec;
	DWORD		nAvgBytesPerSec;
	WORD		nBlockAlign;
	WORD		wBitsPerSample;
	FOURCC		dataType;
	DWORD		dataSize;
	char		theData[];
} PCMWaveRec, *PCMWavePtr, **PCMWaveHnd;

typedef struct _CK {
	FOURCC		ckid;
	DWORD		cksize;
} CK;

typedef struct _MyAtom {
	int			pos;
	int			id;
	int			size;
	FSIORefNum	ref;
} MyAtom;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif
#ifndef __LP64__
__private_extern Ptr ConvertWAV(FSSpec *fileSpec, int *loopStart, int *loopEnd, short *sampleSize, unsigned long *rate, bool *stereo) DEPRECATED_ATTRIBUTE; //Returns memory allocated using Carbon's NewPtr.
__private_extern MADErr ConvertDataToWAVE(FSSpec file, FSSpec *newfile, PPInfoPlug *thePPInfoPlug) DEPRECATED_ATTRIBUTE;
#endif
__private_extern MADErr TestWAV(const PCMWavePtr CC);
__private_extern void *ConvertWAVCFURL(CFURLRef theURL, size_t *sndSize, int *loopStart, int *loopEnd, short *sampleSize, unsigned int *rate, bool *stereo)
;
#ifdef __cplusplus
}
#endif

#endif
