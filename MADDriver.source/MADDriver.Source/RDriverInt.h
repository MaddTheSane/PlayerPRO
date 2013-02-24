/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

#ifndef __RDRIVERINTH__
#define __RDRIVERINTH__

#include <stdio.h>
#ifndef __PPC_FILEUTILS_H__
#include "FileUtils.h"
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

#ifdef LINUX
#include <alsa/asoundlib.h>
#endif

#ifdef _ESOUND
#include <esd.h>
#endif

#ifdef _OSSSOUND
#warning headers not included!
//TODO: include headers
#endif

//#define MEM {Str255 str; NumToString( FreeMem(), str); DebugStr( str);}

//#define NUMBER_FINETUNES 	16

#define AMIGA_CLOCKFREQ2			14317456L

#if defined(HAS_LONG_LONG) && defined(HAS_LONG_DOUBLE)
// Work on 64bits for much better precision
#define BYTEDIV										16L			
#else
#define BYTEDIV										8L
#endif

#define	EXTRASMALLCOUNTER					5

//#define POSPITCH		170
//#define	SoundActive  	0x27E
//#define ASCBase 		0x0CC0

	/**  Some #define functions for easiest access **/

#define SampleDataN(InsD,NoteD)			curMusic->sample[ curMusic->fid[ InsD].firstSample + curMusic->fid[ InsD].what[ NoteD]]
#define SampleDataS(InsD,SampleD)		curMusic->sample[ curMusic->fid[ InsD].firstSample + SampleD]
#define SampleDataD(InsD)						curMusic->sample[ curMusic->fid[ InsD].firstSample + curSample[ InsD]]

#ifdef __cplusplus
extern "C" {
#endif

/*** General Functions ***/

PPEXPORT OSErr 		MADKillInstrument( MADMusic*, short ins);
PPEXPORT OSErr 		MADKillSample( MADMusic *, short ins, short sample);
PPEXPORT sData		*MADCreateSample( MADMusic *MDriver, short ins, short sample);
PPEXPORT OSErr 		MADKillCmd( Cmd*);
PPEXPORT void		UpdateTracksNumber( MADDriverRec *);

PPEXPORT OSErr 		MADCreateVolumeTable( MADDriverRec *intDriver);
PPEXPORT void		MADDisposeVolumeTable( MADDriverRec *intDriver);
PPEXPORT MADMusic*	CreateFreeMADK( void);


	//TODO: either migrate all these functions to PlayerPROCore or remove them from the header.
void AllNoteOff( MADDriverRec *intDriver);
Boolean	MADIsPressed( unsigned char* km2, unsigned short k);
OSErr MADCreateTiming( MADDriverRec *intDriver);
OSErr MADCreateDriverBuffer( MADDriverRec *intDriver);
void MADDisposeDriverBuffer( MADDriverRec *intDriver);
OSErr MADCreateReverb( MADDriverRec *intDriver);
void MADDisposeReverb( MADDriverRec *intDriver);
OSErr MADCreateMicroDelay( MADDriverRec *intDriver);
PPEXPORT size_t MADGetMusicSize( MADMusic*);
OSErr MADReadMAD( MADMusic **music, UNFILE srcFile, short InPutType, Handle MADRsrc, Ptr MADPtr);
OSErr	DBSndClose(MADDriverRec *inMADDriver);
void	StopChannel(MADDriverRec *inMADDriver);
void	PlayChannel(MADDriverRec *inMADDriver);
#if 0
void	InstallMODVBL(MADDriverRec *MDriver);
void	RemoveMODVBL(MADDriverRec *MDriver);
#endif
void MADPurgeTrack( MADDriverRec *intDriver);
PPEXPORT Boolean DirectSave( Ptr myPtr, MADDriverSettings *driverType, MADDriverRec *intDriver);
PPEXPORT Boolean DirectSaveAlways( Ptr myPtr, MADDriverSettings *driverType, MADDriverRec *intDriver);
//void	InstallDMAVBL(MADDriverRec *MDriver);
//void	RemoveDMAVBL(void);
OSErr MADCreateVibrato(MADDriverRec *MDriver);
PPEXPORT PatData* DecompressPartitionMAD1( MADMusic *MDriver, PatData* myPat);
PPEXPORT PatData* CompressPartitionMAD1( MADMusic *MDriver, PatData* myPat);
//Ptr 	DoMAC3Expand( Ptr sound, long *InOutSize);
//Ptr 	DoMAC3Compress( Ptr sound, long *InOutSize);
//void 	DMAIntInstall( UInt32 refCon, Ptr handlerAddr, UInt32 vectorBit);
//void 	DMAIntRemove( UInt32 vectorBit);
PPEXPORT void 	GenerateSound( MADDriverRec *intDriver);
PPEXPORT SInt32	GetOldPeriod( short note, SInt32 c2spd, MADDriverRec *intDriver);
PPEXPORT OSErr MADResetInstrument( InstrData		*curIns);
PPEXPORT void MADCheckSpeed( MADMusic *MDriver, MADDriverRec *intDriver);
PPEXPORT OSErr AddSoundToMAD(	Ptr				theSound,
						SInt32			lS,
						SInt32			lE,
						short			sS,
						short			bFreq,
						UInt32			rate,
						Boolean			stereo,
						Str255			name,
						short			ins,
						short			*sampleID);

PPEXPORT OSErr	MADCopyCurrentPartition( MADMusic	*theNewMAD);
SInt32 DoVolPanning( short, Channel *ch, MADDriverRec *intDriver);
SInt32 DoVolPanning256( short, Channel *ch, MADDriverRec *intDriver, Boolean);
void MADKeyOFF( MADDriverRec *MDriver, short track);

PPEXPORT SInt32 MADMinimize( MADMusic*);
void MADPurgeTrackIfInstru( MADDriverRec *intDriver, short instru);
void MADTickLoopFill8( Channel *curVoice, SInt32 *ASCBuffer1, SInt32 *ASCBuffer2, size_t size, short left, short right);
void MADTickLoop8( size_t size, Channel *curVoice, SInt32 *ASCBuffer1, SInt32 *ASCBuffer2, MADDriverRec *intDriver);
void MADTickRemoverStart8( Channel *curVoice, SInt32	*ASCBuffer1, SInt32	*ASCBuffer2, MADDriverRec *intDriver);
void MADTickRemoverLoop16( SInt32 size, Channel *curVoice, SInt32 *ASCBuffer1, SInt32 *ASCBuffer2, MADDriverRec *intDriver, SInt32 diffL, SInt32 diffR);
void MADTickRemoverLoop8( SInt32 size, Channel *curVoice, SInt32 *ASCBuffer1, SInt32 *ASCBuffer2, MADDriverRec *intDriver, SInt32 diff);

short MADGetNextReader( MADMusic *music, MADDriverRec *intDriver, short cur, short *pat);
PPEXPORT OSErr MADCleanCurrentMusic( MADMusic *MDriver, MADDriverRec *intDriver);
void CloseEffect( Channel *ch, short call, MADDriverRec *intDriver);
SInt32 Interpolate(SInt32 p, SInt32 p1, SInt32 p2, SInt32 v1, SInt32 v2);
SInt32 InterpolateEnv(SInt32 p, EnvRec *a,EnvRec *b);
void ProcessFadeOut( Channel *ch, MADDriverRec *intDriver);
void ProcessEnvelope( Channel *ch, MADDriverRec *intDriver, Boolean);
void StartEnvelope( Channel *ch);
//void LoadImportPLUG( MADLibrary		*inMADDriver, short	No, FSSpec	*theSpec);
void StartPanning( Channel *ch);
void SetUpCmdEffect( Channel *ch, MADDriverRec *intDriver);
OSErr MADInitEqualizer( MADDriverRec *intDriver);
void MADCloseEqualizer( MADDriverRec *intDriver);
void MADrealft(double *data,int n,int isign);
void MADCallFFT( sData *SData, double *filter, MADDriverRec *intDriver, Boolean);
void FFT16S( short* SData, size_t size, double *filter, MADDriverRec *intDriver, short nochan, Boolean);
void FFT8S( char	*SData, size_t size, double *filter, MADDriverRec *intDriver, short nochan, Boolean);
void IntNoteOff( Channel *curVoice, MADDriverRec *intDriver);
Boolean NewMADCommand( Cmd *theNoteCmd);
short FindAFreeChannel( MADDriverRec *intDriver);

// Effects.c

void parse_slidevol(Channel *ch, Byte Arg);
void ConvertTo64Rows( MADMusic *music);

// MIDI
void InitMIDIHarware(void);
void OpenMIDIHardware( MADDriverRec *rec);
void CloseMIDIHarware(void);
	
#ifdef __cplusplus
}
#endif

struct __MADDriverRec
{
	/**********************/
	/** Public variables **/
	/**********************/
	
	MADDriverSettings		DriverSettings;									// Driver SetUp -- READ ONLY --
	
	/**  Current music in memory, loaded with RLoadMusic() by example **/
	
	MADMusic				*curMusic;										// Current music played by this driver, it can be NULL !!!
	MADLibrary				*lib;
	
	/**  Drivers variables **/
	
	Channel					chan[ MAXTRACK];								// Current driver channels -- READ ONLY --
	Boolean					musicEnd;										// Is music playing finished? -- READ ONLY --
	short					Tube[ MAXTRACK];								// Used in 'Tracks View' Window - View menu 
	short					PartitionReader;								// Current position in pattern (0...999)
	short					Pat;											// Current ID Pattern, see 'Patterns list'
	short					PL;												// Current position in partition, see 'Partition list'
	short					VolGlobal;										// Global SOFTWARE volume (This is NOT Mac hardware volume!) from 0 to 64
	short					speed;											// Current speed, see speed Effect
	short					finespeed;										// Current finespeed, see speed Effect
	short					InstruTube[ MAXINSTRU];							// Used in 'Instrument View' Window - View menu
	short					VExt;											// External music speed, see 'Adaptators' window. 80 = normal
	short					FreqExt;										// External music pitch, see 'Adaptators' window. 80 = normal
	Boolean					Reading;										// Reading indicator
	Boolean					Active[ MAXTRACK];								// Channel Active?
	Boolean					Equalizer;										// Is Equalizer Active?
	
#ifdef _MAC_H
	AudioUnit				CAAudioUnit;
	UInt32					CABufOff;
	Ptr						CABuffer;
#endif
	
#ifdef WIN32
	LPDIRECTSOUND			lpDirectSound;									// The LPDIRECTSOUND to apply & get informations, etc.
	LPDIRECTSOUNDBUFFER		lpDirectSoundBuffer, lpSwSamp;					// ONLY available if you are using Win95 DirectSound driver
	Ptr 					currentBuf;
	Boolean					OnOff;
	long					WIN95BUFFERSIZE;
	UINT					gwID;
	HWND					hwnd;
	WAVEOUTCAPS				woc;
	HWAVEOUT 				hWaveOut;
	LPVOID 					mydata;
	HGLOBAL 				hglobal;
	WAVEHDR 				WaveOutHdr;
	char 					*mydma;
	long					MICROBUFState;
#endif
	
#ifdef _OSSSOUND
	//TODO: OSS Sound Driver
#endif
	
#ifdef LINUX
	//TODO: ALSA Sound Driver
#endif
	
#ifdef _ESOUND
	//TODO: EsounD driver
#endif
	
	Ptr						OscilloWavePtr;									// Contains actual sound wave of music, in char (8 bits) or in short (16 bits)
	size_t					OscilloWaveSize;								// Size of previous buffer
	
	/** Private variables - Not documented **/
	/* DO NOT MODIFY OR USE these variables */
	
	SInt32					BytesToRemoveAtEnd, MIN_PITCH, MAX_PITCH, MOD_MIN_PITCH, MOD_MAX_PITCH, ASCBUFFERReal;
	short					smallcounter, trackDiv;
	SInt32					FREQBASE;
	short					InstruActif[ MAXINSTRU];
	Ptr						IntDataPtr;
	Ptr						OsciDrawPtr[ MAXDRAW];
	Boolean					newData[ MAXDRAW];
	Boolean					useOsciBuffers;
	short					curDrawPtr;
	unsigned long			curTime;
	Boolean					XMLinear, MODMode, JumpToNextPattern, endPattern, MADPlay;
	SInt32					ASCBUFFER;
	size_t					BufSize;
	SInt32					VSYNC, BufCounter, BytesToGenerate;
	short					vibrato_table[ 64];
	short					SendMIDIClockData;	//gOutNodeRefNum, MIDIPortRefNum
	short					InstuNoOld[ MAXTRACK];
	short					NoteOld[ MAXTRACK];
	short					VelocityOld[ MAXTRACK];
	Boolean					TrackLineReading[ MAXTRACK], TrackReading[ MAXTRACK], wasReading;
	Ptr						OverShoot;
	SInt32					*DASCBuffer;//, *DASCBufferOut;
	SInt32					*DASCEffectBuffer[ MAXCHANEFFECT];
	SInt32					EffectBufferID[ MAXCHANEFFECT];
	SInt32					EffectBufferRealID[ MAXCHANEFFECT];
	short					*DASCBuffer8;//, *DASCBuffer8Out;
	double					*Filter, *fData;
	SInt32					MDelay;
	SInt32					RDelay;
	Ptr						ReverbPtr;
	short					PatDelay;
	short					lastChannelUsed[ MAXTRACK];
	SInt32					MultiChanNo, globPan;
	Boolean					currentlyExporting;
	Boolean					thisExport;
	Boolean					OneMoreBeforeEnd;
	Boolean					clipL, clipR;
	SInt32					levelL, levelR;
	SInt32					curCenterL, curCenterR;
	
	
//#ifdef _MAC_H
	VSTEffect				*masterVST[ 10];
	VSTEffect				*chanVST[ MAXTRACK][ 4];
	//PPSndDoubleBufferHeader 	TheHeader;
//#endif
	
};
	

#endif
