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

//#define MEM {Str255 str; NumToString( FreeMem(), str); DebugStr( str);}

//#define NUMBER_FINETUNES 	16

#define AMIGA_CLOCKFREQ2			14317456L

//#if defined(powerc) || defined (__powerc) || defined(__ppc__)
// Work on 64bits for much better precision
#define BYTEDIV										16L			
//#else
//#define BYTEDIV										8L
//#endif

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

/********************						***********************/
/*** 					INTERNAL ROUTINES						***/
/***			    DO NOT use these routines					***/
/********************						***********************/

/*** Deluxe ***/

void	MADCreateOverShoot( MADDriverRec *intDriver);
void	MADKillOverShoot( MADDriverRec *intDriver);
void 	Sampler16AddDeluxe( Channel *curVoice, register SInt32	*ASCBuffer, MADDriverRec *intDriver);
void 	Sampler16Addin16Deluxe( Channel *curVoice, register SInt32	*ASCBuffer, MADDriverRec *intDriver);
void 	Sample16BufferAddDeluxe( Channel *curVoice, register SInt32	*ASCBuffer, MADDriverRec *intDriver);
void 	Play16StereoDeluxe( MADDriverRec *intDriver);
void 	Sampler8in8AddDeluxe( Channel *curVoice, register short	*ASCBuffer, MADDriverRec *intDriver);
void 	Sampler8in16AddDeluxe( Channel *curVoice, register short	*ASCBuffer, MADDriverRec *intDriver);
void 	Sample8BufferAddDeluxe( Channel *curVoice, register short *ASCBuffer, MADDriverRec *intDriver);
void 	Play8StereoDeluxe( MADDriverRec *intDriver);

/*** Delay ***/

void Sampler16AddDelay( Channel *curVoice, register SInt32	*ASCBuffer, MADDriverRec *intDriver);
void Sampler16Addin16Delay( Channel *curVoice, register SInt32	*ASCBuffer, MADDriverRec *intDriver);
void Sample16BufferAddDelay( Channel *curVoice, register SInt32	*ASCBuffer, MADDriverRec *intDriver);
void Play16StereoDelay( MADDriverRec *intDriver);
void Sampler8in8AddDelay( Channel *curVoice, register short	*ASCBuffer, MADDriverRec *intDriver);
void Sampler8in16AddDelay( Channel *curVoice, register short	*ASCBuffer, MADDriverRec *intDriver);
void Sample8BufferAddDelay( Channel *curVoice, register short *ASCBuffer, MADDriverRec *intDriver);
void Play8StereoDelay( MADDriverRec *intDriver);

/*** 8 Bits ***/

void 	Sampler8in8Add( Channel *curVoice, register Ptr	ASCBuffer, MADDriverRec *intDriver);
void 	Sampler8in16Add( Channel *curVoice, register Ptr	ASCBuffer, MADDriverRec *intDriver);
void 	Sampler8in8AddPoly( Channel *curVoice, register Ptr	ASCBuffer, short chanNo, MADDriverRec *intDriver);
void 	Sampler8in16AddPoly( Channel *curVoice, register Ptr	ASCBuffer, short chanNo, MADDriverRec *intDriver);
void 	Sample8BufferAdd( Channel *curVoice, register Ptr ASCBuffer, MADDriverRec *intDriver);
void 	Sample8BufferAddPoly( Channel *curVoice, register Ptr ASCBuffer, short chanNo, MADDriverRec *intDriver);
void	Play8Mono( MADDriverRec *intDriver);
void 	Play8Stereo( MADDriverRec *intDriver);
void 	Play8PolyPhonic( MADDriverRec *intDriver);

/*** 16 Bits ***/

void 	Sampler16Add( Channel *curVoice, register short	*ASCBuffer, MADDriverRec* intDriver);
void 	Sampler16Addin16( Channel *curVoice, register short	*ASCBuffer, MADDriverRec* intDriver);
void 	Sample16BufferAdd( Channel *curVoice, register short	*ASCBuffer, MADDriverRec* intDriver);
void 	Play16Stereo( MADDriverRec* intDriver);
void 	Sampler16AddPoly( Channel *curVoice, register short	*ASCBuffer, short chanNo, MADDriverRec* intDriver);
void 	Sampler16Addin16Poly( Channel *curVoice, register short	*ASCBuffer, short chanNo, MADDriverRec* intDriver);
void 	Sample16BufferAddPoly( Channel *curVoice, register short	*ASCBuffer, short chanNo, MADDriverRec* intDriver);
void 	Play16PolyPhonic( MADDriverRec* intDriver);
void 	Play16Mono( MADDriverRec* intDriver);

/*** 20 Bits ***/
//TODO

/*** 24 Bits ***/
//TODO

/*** Effects ***/

void 	DoEffect( Channel *ch, short call, MADDriverRec *intDriver);
void 	SetUpEffect( Channel *ch, MADDriverRec *intDriver);
void DoVolCmd( Channel *ch, short call, MADDriverRec *intDriver);

/*** Interruption Functions ***/

void 	NoteAnalyse( MADDriverRec *intDriver);
void 	ReadNote( Channel *curVoice, Cmd		*theCommand, MADDriverRec *intDriver);
void 	Filter8Bit( register Byte *myPtr, MADDriverRec *intDriver);
void 	Filter8BitX( register Byte *myPtr, MADDriverRec *intDriver);
void 	Filter16BitX( register short *myPtr, MADDriverRec *intDriver);
void 	BufferCopyM( MADDriverRec *intDriver);
void 	BufferCopyS( MADDriverRec *intDriver);
void 	NoteOff(short oldIns, short oldN, short oldV, MADDriverRec *intDriver);
void 	SampleMIDI( Channel *curVoice, short channel, short curN, MADDriverRec *intDriver);
void 	CleanDriver( MADDriverRec *intDriver);

/*** General Functions ***/

PPEXPORT OSErr 		MADKillInstrument( MADMusic*, short ins);
PPEXPORT OSErr 		MADKillSample( MADMusic *, short ins, short sample);
PPEXPORT sData			*MADCreateSample( MADMusic *MDriver, short ins, short sample);
PPEXPORT OSErr 		MADKillCmd( Cmd*);
PPEXPORT void			UpdateTracksNumber( MADDriverRec *);

PPEXPORT OSErr 		MADCreateVolumeTable( MADDriverRec *intDriver);
PPEXPORT void			MADDisposeVolumeTable( MADDriverRec *intDriver);

	//TODO: either migrate all these functions to PlayerPROCore or remove them from the header.
void AllNoteOff( MADDriverRec *intDriver);
Boolean	MADIsPressed( unsigned char* km2, unsigned short k);
OSErr MADCreateTiming( MADDriverRec *intDriver);
OSErr MADCreateDriverBuffer( MADDriverRec *intDriver);
void MADDisposeDriverBuffer( MADDriverRec *intDriver);
OSErr MADCreateReverb( MADDriverRec *intDriver);
void MADDisposeReverb( MADDriverRec *intDriver);
OSErr MADCreateMicroDelay( MADDriverRec *intDriver);
size_t MADGetMusicSize( MADMusic*);
OSErr MADReadMAD( MADMusic **music, UNFILE srcFile, short InPutType, Handle MADRsrc, Ptr MADPtr);
OSErr	DBSndClose(MADDriverRec *inMADDriver);
void	StopChannel(MADDriverRec *inMADDriver);
void	PlayChannel(MADDriverRec *inMADDriver);
#if 0
void	InstallMODVBL(MADDriverRec *MDriver);
void	RemoveMODVBL(MADDriverRec *MDriver);
#endif
void	MODRelance(void);
void	Play(void);
void MADPurgeTrack( MADDriverRec *intDriver);
PPEXPORT Boolean DirectSave( Ptr myPtr, MADDriverSettings *driverType, MADDriverRec *intDriver);
PPEXPORT Boolean DirectSaveAlways( Ptr myPtr, MADDriverSettings *driverType, MADDriverRec *intDriver);
void	ChangeSpeed( void);
void 	ClearFrequence(void);
//void	InstallDMAVBL(MADDriverRec *MDriver);
//void	RemoveDMAVBL(void);
OSErr MADCreateVibrato(MADDriverRec *MDriver);
PatData* DecompressPartitionMAD1( MADMusic *MDriver, PatData* myPat);
PatData* CompressPartitionMAD1( MADMusic *MDriver, PatData* myPat);
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

OSErr	MADCopyCurrentPartition( MADMusic	*theNewMAD);
SInt32 DoVolPanning( short, Channel *ch, MADDriverRec *intDriver);
SInt32 DoVolPanning256( short, Channel *ch, MADDriverRec *intDriver, Boolean);
void MADKeyOFF( MADDriverRec *MDriver, short track);

SInt32 MADMinimize( MADMusic*);
void MADPurgeTrackIfInstru( MADDriverRec *intDriver, short instru);
void MADTickLoopFill8( Channel *curVoice, SInt32 *ASCBuffer1, SInt32 *ASCBuffer2, size_t size, short left, short right);
void MADTickLoop8( size_t size, Channel *curVoice, SInt32 *ASCBuffer1, SInt32 *ASCBuffer2, MADDriverRec *intDriver);
void MADTickRemoverStart8( Channel *curVoice, SInt32	*ASCBuffer1, SInt32	*ASCBuffer2, MADDriverRec *intDriver);
void MADTickRemoverLoop16( SInt32 size, Channel *curVoice, SInt32 *ASCBuffer1, SInt32 *ASCBuffer2, MADDriverRec *intDriver, SInt32 diffL, SInt32 diffR);
void MADTickRemoverLoop8( SInt32 size, Channel *curVoice, SInt32 *ASCBuffer1, SInt32 *ASCBuffer2, MADDriverRec *intDriver, SInt32 diff);

short MADGetNextReader( MADMusic *music, MADDriverRec *intDriver, short cur, short *pat);
OSErr MADCleanCurrentMusic( MADMusic *MDriver, MADDriverRec *intDriver);
void CloseEffect( Channel *ch, short call, MADDriverRec *intDriver);
//void MADSetMADDriverPtr( MADDriverRec* driver);
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
	//TODO: CoreAudio Driver
	AudioUnit				CAAudioUnit;
#endif
	
#ifdef WIN32
	LPDIRECTSOUND			lpDirectSound;									// The LPDIRECTSOUND to apply & get informations, etc.
	LPDIRECTSOUNDBUFFER		lpDirectSoundBuffer, lpSwSamp;					// ONLY available if you are using Win95 DirectSound driver
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
