//
//  MADDriver.h
//  PPMacho
//
//  Created by C.W. Betts on 9/4/13.
//
//

#ifndef PPMacho_MADDriver_h
#define PPMacho_MADDriver_h

#ifndef __RDRIVERH__
#include "RDriver.h"
#endif

#ifdef WIN32
#ifndef __DSOUND_INCLUDED__
#include <mmreg.h>
#include <DSound.h>
#endif
#endif

#ifdef _MAC_H
#include <AudioUnit/AudioUnit.h>
#endif

#ifdef _ESOUND
#include <esd.h>
#endif

#ifndef BUILDINGPPRO
#warning this header should NOT be used outside of PlayerPROCore! The functions and datatypes here may change FOR ANY REASON, including differing compiler includes and defines.
#endif

struct __MADDriverRec {
	/**********************/
	/** Public variables **/
	/**********************/
	
	MADDriverSettings		DriverSettings;									// Driver SetUp -- READ ONLY --
	
	/**  Current music in memory, loaded with RLoadMusic() by example **/
	
	MADMusic		*curMusic;										// Current music played by this driver, it can be NULL !!!
	MADLibrary		*lib;
	
	/**  Drivers variables **/
	
	Channel			chan[MAXTRACK];									// Current driver channels -- READ ONLY --
	Boolean			musicEnd;										// Is music playing finished? -- READ ONLY --
	short			Tube[MAXTRACK];									// Used in 'Tracks View' Window - View menu
	short			PartitionReader;								// Current position in pattern (0...999)
	short			Pat;											// Current ID Pattern, see 'Patterns list'
	short			PL;												// Current position in partition, see 'Partition list'
	short			VolGlobal;										// Global SOFTWARE volume (This is NOT Mac hardware volume!) from 0 to 64
	short			speed;											// Current speed, see speed Effect
	short			finespeed;										// Current finespeed, see speed Effect
	short			InstruTube[MAXINSTRU];							// Used in 'Instrument View' Window - View menu
	short			VExt;											// External music speed, see 'Adaptators' window. 80 = normal
	short			FreqExt;										// External music pitch, see 'Adaptators' window. 80 = normal
	Boolean			Reading;										// Reading indicator
	Boolean			Active[MAXTRACK];								// Channel Active?
	Boolean			Equalizer;										// Is Equalizer Active?
	
	void			*OscilloWavePtr;								// Contains actual sound wave of music, in char (8 bits) or in short (16 bits)
	size_t			OscilloWaveSize;								// Size of previous buffer
	
	/** Private variables - Not documented **/
	/* DO NOT MODIFY OR USE these variables */
	
	SInt32			BytesToRemoveAtEnd, MIN_PITCH, MAX_PITCH, MOD_MIN_PITCH, MOD_MAX_PITCH, ASCBUFFERReal;
	short			smallcounter, trackDiv;
	SInt32			FREQBASE;
	short			InstruActif[ MAXINSTRU];
	char			*IntDataPtr;
	void			*OsciDrawPtr[ MAXDRAW];
	Boolean			newData[ MAXDRAW];
	Boolean			useOsciBuffers;
	short			curDrawPtr;
	unsigned long	curTime;
	Boolean			XMLinear, MODMode, JumpToNextPattern, endPattern, MADPlay;
	SInt32			ASCBUFFER;
	size_t			BufSize;
	SInt32			VSYNC, BufCounter, BytesToGenerate;
	short			vibrato_table[64];
	short			SendMIDIClockData;	//gOutNodeRefNum, MIDIPortRefNum
	short			InstuNoOld[MAXTRACK];
	short			NoteOld[MAXTRACK];
	short			VelocityOld[MAXTRACK];
	Boolean			TrackLineReading[MAXTRACK], TrackReading[MAXTRACK], wasReading;
	char			*OverShoot;
	SInt32			*DASCBuffer;//, *DASCBufferOut;
	SInt32			*DASCEffectBuffer[MAXCHANEFFECT];
	SInt32			EffectBufferID[MAXCHANEFFECT];
	SInt32			EffectBufferRealID[MAXCHANEFFECT];
	short			*DASCBuffer8;//, *DASCBuffer8Out;
	double			*Filter, *fData;
	SInt32			MDelay;
	SInt32			RDelay;
	void			*ReverbPtr;
	short			PatDelay;
	short			lastChannelUsed[MAXTRACK];
	SInt32			MultiChanNo, globPan;
	Boolean			currentlyExporting;
	Boolean			thisExport;
	Boolean			OneMoreBeforeEnd;
	Boolean			clipL, clipR;
	SInt32			levelL, levelR;
	SInt32			curCenterL, curCenterR;
	
	
	VSTEffect		*masterVST[ 10];
	VSTEffect		*chanVST[ MAXTRACK][ 4];
	
#ifdef _MAC_H
	AudioUnit		CAAudioUnit;
	size_t			CABufOff;
	char			*CABuffer;
#endif
	
#ifdef WIN32
	LPDIRECTSOUND		lpDirectSound; // The LPDIRECTSOUND to apply & get informations, etc.
	LPDIRECTSOUNDBUFFER	lpDirectSoundBuffer, lpSwSamp; // ONLY available if you are using Win95 DirectSound driver
	Ptr 				currentBuf;
	Boolean				OnOff;
	long				WIN95BUFFERSIZE;
	UINT				gwID;
	HWND				hwnd;
	WAVEOUTCAPS			woc;
	HWAVEOUT 			hWaveOut;
	LPVOID 				mydata;
	HGLOBAL 			hglobal;
	WAVEHDR 			WaveOutHdr;
	char 				*mydma;
	long				MICROBUFState;
#endif
	
#ifdef _ESOUND
	//TODO: EsounD driver
#endif
	
};


#endif
