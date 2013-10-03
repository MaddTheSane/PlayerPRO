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

#if MAINPLAYERPRO
#include "PPDefs.h"
#else
#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/PPDefs.h>
#include <PlayerPROCore/PPPlug.h>
#endif

typedef struct _infoformat_tag{
	Str255	name;
	Str32	smpls;
	Str32	dataBits;
	Str32	stereo;
} InfoRec, *InfoPtr, **InfoHnd;

typedef struct _MMCKINFO{
	FOURCC		ckid;
	DWORD		cksize;
	FOURCC		fccType;
	DWORD		dwDataOffset;
	WORD		dwFlags;
} MMCKINFO;

typedef struct waveformat_tag{
	WORD		wFormatTag;
	WORD		nCannels;
	DWORD		nSamplesPerSec;
	DWORD		nAvgBytesPerSec;
	WORD		nBlockAlign;
	WORD		wBitsPerSample;
} WAVEFORMAT;

typedef struct _pcwaveformat_tag{
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

typedef struct _CK{
	FOURCC		ckid;
	DWORD		cksize;
} CK;

typedef struct _MyAtom{
	long		pos;
	long		id;
	long		size;
	short		ref;
} MyAtom;


#ifdef __cplusplus
extern "C" {
#endif

OSErr TestWAV( PCMWavePtr CC);
Ptr ConvertWAV(FSSpec *fileSpec, long *loopStart, long *loopEnd, short	*sampleSize, unsigned long *rate, Boolean *stereo);
OSErr ConvertDataToWAVE( FSSpec file, FSSpec *newfile, PPInfoPlug *thePPInfoPlug);

#ifdef __cplusplus
}
#endif		

#endif
