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
#endif

typedef struct _infoformat_tag{
	Str255	name;
	Str32	smpls;
	Str32	dataBits;
	Str32	stereo;
} InfoRec;
typedef InfoRec *InfoPtr;
typedef InfoPtr *InfoHnd;

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
} PCMWaveRec;
typedef PCMWaveRec *PCMWavePtr;
typedef PCMWavePtr *PCMWaveHnd;

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

#endif
