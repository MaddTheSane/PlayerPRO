//
//  MADDriver.h
//  PPMacho
//
//  Created by C.W. Betts on 9/4/13.
//
//

#ifndef PPMacho_MADDriver_h
#define PPMacho_MADDriver_h

#include "RDriver.h"

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

#ifdef HAVE_PULSEAUDIO
#warning TODO: import PulseAudio headers
#endif

#ifdef HAVE_PORTAUDIO
#warning TODO: import PortAudio headers
#endif

#ifndef BUILDINGPPRO
#warning this header should NOT be used outside of PlayerPROCore! The functions and datatypes here may change FOR ANY REASON, including differing compiler includes and defines.
#endif

struct MADDriverRec {
	MADDriverBase	base;
	
	/** Private variables - Not documented **/
	/* DO NOT MODIFY OR USE these variables */
	
	int				BytesToRemoveAtEnd, MIN_PITCH, MAX_PITCH, MOD_MIN_PITCH, MOD_MAX_PITCH, ASCBUFFERReal;
	short			smallcounter, trackDiv;
	int				FREQBASE;
	short			InstruActif[MAXINSTRU];
	char			*IntDataPtr;
	void			*OsciDrawPtr[MAXDRAW];
	bool			newData[MAXDRAW];
	bool			useOsciBuffers;
	short			curDrawPtr;
	unsigned long	curTime;
	bool			XMLinear, MODMode, JumpToNextPattern, endPattern, MADPlay;
	int				ASCBUFFER;
	size_t			BufSize;
	int				VSYNC, BufCounter, BytesToGenerate;
	short			vibrato_table[64];
	short			SendMIDIClockData;	//gOutNodeRefNum, MIDIPortRefNum
	short			InstuNoOld[MAXTRACK];
	short			NoteOld[MAXTRACK];
	short			VelocityOld[MAXTRACK];
	bool			TrackLineReading[MAXTRACK], TrackReading[MAXTRACK], wasReading;
	char			*OverShoot;
	int				*DASCBuffer;//, *DASCBufferOut;
	int				*DASCEffectBuffer[MAXCHANEFFECT];
	int				EffectBufferID[MAXCHANEFFECT];
	int				EffectBufferRealID[MAXCHANEFFECT];
	short			*DASCBuffer8;//, *DASCBuffer8Out;
	double			*Filter, *fData;
	int				MDelay;
	int				RDelay;
	void			*ReverbPtr;
	short			PatDelay;
	short			lastChannelUsed[MAXTRACK];
	int				MultiChanNo, globPan;
	bool			currentlyExporting;
	bool			thisExport;
	bool			OneMoreBeforeEnd;
	bool			clipL, clipR;
	int				levelL, levelR;
	int				curCenterL, curCenterR;
	
	bool			hasVSTEnabled;
	void			*vstEffects;
	VSTEffect		*masterVST[10];
	VSTEffect		*chanVST[MAXTRACK][4];
	
#ifdef _MAC_H
	AudioUnit		CAAudioUnit;
	size_t			CABufOff;
	char			*CABuffer;
#endif
	
#ifdef WIN32
	LPDIRECTSOUND		lpDirectSound;					// The LPDIRECTSOUND to apply & get informations, etc.
	LPDIRECTSOUNDBUFFER	lpDirectSoundBuffer, lpSwSamp;	// ONLY available if you are using Win95 DirectSound driver
	char* 				currentBuf;
	bool				OnOff;
	size_t				WIN95BUFFERSIZE;
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
	// TODO: EsounD driver
#endif
	
#ifdef HAVE_PULSEAUDIO
	// TODO: PulseAudio driver
#endif
	
#ifdef HAVE_PORTAUDIO
	// TODO: PortAudio driver
#endif
};


#endif
