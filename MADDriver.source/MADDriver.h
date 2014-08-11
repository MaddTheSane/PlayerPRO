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

#ifdef _SDL
#include <SDL.h>
#include <SDL_Audio.h>
#if !SDL_IS_LINKED
typedef int (SDLCALL *SDL_OpenAudioPtr)(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
typedef void (SDLCALL *SDL_CloseAudioPtr)();
typedef int (SDLCALL *SDL_InitPtr)(Uint32);
typedef int (SDLCALL *SDL_InitSubPtr)(Uint32);
typedef Uint32 (SDLCALL *SDL_WasInitPtr)(Uint32 flags);
typedef void (SDLCALL *SDL_QuitSubSystemPtr)(Uint32 flags);
typedef void (SDLCALL *SDL_QuitPtr)(void);
#endif
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
	bool			musicEnd;										// Is music playing finished? -- READ ONLY --
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
	bool			Reading;										// Reading indicator
	bool			Active[MAXTRACK];								// Channel Active?
	bool			Equalizer;										// Is Equalizer Active?
	
	void			*OscilloWavePtr;								// Contains actual sound wave of music, in char (8 bits) or in short (16 bits)
	size_t			OscilloWaveSize;								// Size of previous buffer
	
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
	
	MADBool			hasVSTEnabled;
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
	MADBool				OnOff;
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
	//TODO: EsounD driver
#endif
	
#ifdef _SDL
	char	*SDLBuffer;
	size_t	SDLBufferOffset;
#if !SDL_IS_LINKED
	void*					dlfcnPtr;
	SDL_OpenAudioPtr		SDLOpenAudio;
	SDL_CloseAudioPtr		SDLCloseAudio;
	SDL_InitPtr				SDLInit;
	SDL_InitSubPtr			SDLInitSub;
	SDL_WasInitPtr			SDLWasInit;
	SDL_QuitSubSystemPtr	SDLQuitSubSystem;
	SDL_QuitPtr				SDLQuit;
#endif
#endif
};


#endif
