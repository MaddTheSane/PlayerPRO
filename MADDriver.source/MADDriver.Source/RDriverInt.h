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

#define MEM {Str255 str; NumToString( FreeMem(), str); DebugStr( str);}

//#define NUMBER_FINETUNES 	16

#define AMIGA_CLOCKFREQ2			14317456L

#if defined(powerc) || defined (__powerc) || defined(__ppc__)
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

/********************						***********************/
/*** 					INTERNAL ROUTINES						***/
/***			    DO NOT use these routines					***/
/********************						***********************/

/*** Deluxe ***/

void	MADCreateOverShoot( MADDriverRec *intDriver);
void	MADKillOverShoot( MADDriverRec *intDriver);
void 	Sampler16AddDeluxe( Channel *curVoice, register long	*ASCBuffer, MADDriverRec *intDriver);
void 	Sampler16Addin16Deluxe( Channel *curVoice, register long	*ASCBuffer, MADDriverRec *intDriver);
void 	Sample16BufferAddDeluxe( Channel *curVoice, register long	*ASCBuffer, MADDriverRec *intDriver);
void 	Play16StereoDeluxe( MADDriverRec *intDriver);
void 	Sampler8in8AddDeluxe( Channel *curVoice, register short	*ASCBuffer, MADDriverRec *intDriver);
void 	Sampler8in16AddDeluxe( Channel *curVoice, register short	*ASCBuffer, MADDriverRec *intDriver);
void 	Sample8BufferAddDeluxe( Channel *curVoice, register short *ASCBuffer, MADDriverRec *intDriver);
void 	Play8StereoDeluxe( MADDriverRec *intDriver);

/*** Delay ***/

void Sampler16AddDelay( Channel *curVoice, register long	*ASCBuffer, MADDriverRec *intDriver);
void Sampler16Addin16Delay( Channel *curVoice, register long	*ASCBuffer, MADDriverRec *intDriver);
void Sample16BufferAddDelay( Channel *curVoice, register long	*ASCBuffer, MADDriverRec *intDriver);
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

/*** Effects ***/

void 	DoEffect( Channel *ch, short call, MADDriverRec *intDriver);
void 	SetUpEffect( Channel *ch, MADDriverRec *intDriver);
void DoVolCmd( Channel *ch, short call, MADDriverRec *intDriver);

/*** Interruption Functions ***/

void 	NoteAnalyse( MADDriverRec *intDriver);
pascal 	void DMAPlay ( void);
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

OSErr 		MADKillInstrument( MADMusic*, short ins);
OSErr 		MADKillSample( MADMusic *, short ins, short sample);
sData			*MADCreateSample( MADMusic *MDriver, short ins, short sample);
OSErr 		MADKillCmd( Cmd*);
void			UpdateTracksNumber( MADDriverRec *);

OSErr 		MADCreateVolumeTable( MADDriverRec *intDriver);
void			MADDisposeVolumeTable( MADDriverRec *intDriver);

void AllNoteOff( MADDriverRec *intDriver);
Boolean	MADIsPressed( unsigned char* km2, unsigned short k);
OSErr MADCreateTiming( MADDriverRec *intDriver);
OSErr MADCreateDriverBuffer( MADDriverRec *intDriver);
void MADDisposeDriverBuffer( MADDriverRec *intDriver);
OSErr MADCreateReverb( MADDriverRec *intDriver);
void MADDisposeReverb( MADDriverRec *intDriver);
OSErr MADCreateMicroDelay( MADDriverRec *intDriver);
long MADGetMusicSize( MADMusic*);
OSErr MADReadMAD( MADMusic **music, UNFILE srcFile, short InPutType, Handle MADRsrc, Ptr MADPtr);
OSErr	DBSndClose(MADDriverRec *inMADDriver);
void	StopChannel(MADDriverRec *inMADDriver);
void	PlayChannel(MADDriverRec *inMADDriver);
void	InstallMODVBL(MADDriverRec *MDriver);
void	RemoveMODVBL(MADDriverRec *MDriver);
void	MODRelance(void);
void	Play(void);
void	VIAOn(void);
void 	SndOff(void);
void 	SndOn(void);
void	VIAOn2(void);
void 	VIAOn3(void);
void MADPurgeTrack( MADDriverRec *intDriver);
OSErr	InitDBSoundManager( MADDriverRec *intDriver, long);
EXP Boolean DirectSave( Ptr myPtr, MADDriverSettings *driverType, MADDriverRec *intDriver);
void	ChangeSpeed( void);
void 	ClearFrequence(void);
void	InstallDMAVBL(MADDriverRec *MDriver);
void	RemoveDMAVBL(void);
OSErr MADCreateVibrato(MADDriverRec *MDriver);
void 	ActiveDMA(void);
PatData* DecompressPartitionMAD1( MADMusic *MDriver, PatData* myPat);
PatData* CompressPartitionMAD1( MADMusic *MDriver, PatData* myPat);
Ptr 	DoMAC3Expand( Ptr sound, long *InOutSize);
Ptr 	DoMAC3Compress( Ptr sound, long *InOutSize);
void 	DMAIntInstall( unsigned long refCon, Ptr handlerAddr, unsigned long vectorBit);
void 	DMAIntRemove( unsigned long vectorBit);
void 	GenerateSound( MADDriverRec *intDriver);
long	GetOldPeriod( short note, long c2spd, MADDriverRec *intDriver);
OSErr MADResetInstrument( InstrData		*curIns);
void MADCheckSpeed( MADMusic *MDriver, MADDriverRec *intDriver);
OSErr AddSoundToMAD(	Ptr				theSound,
						long			lS,
						long			lE,
						short			sS,
						short			bFreq,
						unsigned long	rate,
						Boolean			stereo,
						Str255			name,
						short			ins,
						short			*sampleID);

OSErr	PPTestFile( MADLibrary		*inMADDriver, char	*kindFile, char	*AlienFile);
OSErr	PPInfoFile( MADLibrary		*inMADDriver, char	*kindFile, char	*AlienFile, PPInfoRec	*InfoRec);
OSErr	PPExportFile( MADLibrary		*inMADDriver, char	*kindFile, char	*AlienFile, MADMusic	*theNewMAD);
OSErr	PPImportFile( MADLibrary		*inMADDriver, char	*kindFile, char	*AlienFile, MADMusic	**theNewMAD);
OSErr	PPIdentifyFile( MADLibrary		*inMADDriver, char *kindFile, char	*AlienFile);

OSType	GetPPPlugType( MADLibrary		*inMADDriver, short ID, OSType type);
void	MInitImportPlug( MADLibrary		*inMADDriver, char*);
#ifdef _MAC_H
void	MADInitImportPlug( MADLibrary	*inMADDriver, char *PluginFolder);
#endif

void	CloseImportPlug( MADLibrary		*inMADDriver);
OSErr	MADLoadMADFileCString( MADMusic **, Ptr fName);
OSErr	CheckMADFile( char *AlienFile);
OSErr	MADCopyCurrentPartition( MADMusic	*theNewMAD);
OSErr	CallImportPlug( 	MADLibrary		*inMADDriver,
							short			PlugNo,			// CODE du plug
							OSType			order,
							char			*AlienFile,
							MADMusic		*theNewMAD,
							PPInfoRec		*info);
long DoVolPanning( short, Channel *ch, MADDriverRec *intDriver);
long DoVolPanning256( short, Channel *ch, MADDriverRec *intDriver, Boolean);
void MADKeyOFF( MADDriverRec *MDriver, short track);

long MADMinimize( MADMusic*);
void MADPurgeTrackIfInstru( MADDriverRec *intDriver, short instru);
void MADTickLoopFill8( Channel *curVoice, long *ASCBuffer1, long *ASCBuffer2, long size, short left, short right);
void MADTickLoop8( long size, Channel *curVoice, long *ASCBuffer1, long *ASCBuffer2, MADDriverRec *intDriver);
void MADTickRemoverStart8( Channel *curVoice, long	*ASCBuffer1, long	*ASCBuffer2, MADDriverRec *intDriver);
void MADTickRemoverLoop16( long size, Channel *curVoice, long	*ASCBuffer1, long	*ASCBuffer2, MADDriverRec *intDriver, long diffL, long diffR);
void MADTickRemoverLoop8( long size, Channel *curVoice, long *ASCBuffer1, long *ASCBuffer2, MADDriverRec *intDriver, long diff);

short MADGetNextReader( MADMusic *music, MADDriverRec *intDriver, short cur, short *pat);
OSErr MADCleanCurrentMusic( MADMusic *MDriver, MADDriverRec *intDriver);
void CloseEffect( Channel *ch, short call, MADDriverRec *intDriver);
void MADSetMADDriverPtr( MADDriverRec* driver);
long Interpolate(long p,long p1,long p2,long v1,long v2);
long InterpolateEnv(long p, EnvRec *a,EnvRec *b);
void ProcessFadeOut( Channel *ch, MADDriverRec *intDriver);
void ProcessEnvelope( Channel *ch, MADDriverRec *intDriver, Boolean);
void StartEnvelope( Channel *ch);
void NScanResource( MADLibrary *inMADDriver);
void NScanDirImportPlug( MADLibrary		*inMADDriver, long dirID, short VRefNum, Str255	Fname);
void LoadImportPLUG( MADLibrary		*inMADDriver, short	No, FSSpec	*theSpec);
void DirectSoundClose( MADDriverRec* driver);
Boolean DirectSoundInit( MADDriverRec* driver);
Boolean W95_Init( MADDriverRec* driver);
void W95_Exit( MADDriverRec* driver);
void StartPanning( Channel *ch);
void SetUpCmdEffect( Channel *ch, MADDriverRec *intDriver);
OSErr MADInitEqualizer( MADDriverRec *intDriver);
void MADCloseEqualizer( MADDriverRec *intDriver);
void MADrealft(double *data,int n,int isign);
void MADCallFFT( sData *SData, double *filter, MADDriverRec *intDriver, Boolean);
void FFT16S( short* SData, long size, double *filter, MADDriverRec *intDriver, short nochan, Boolean);
void FFT8S( char	*SData, long size, double *filter, MADDriverRec *intDriver, short nochan, Boolean);
void IntNoteOff( Channel *curVoice, MADDriverRec *intDriver);
Boolean NewMADCommand( Cmd *theNoteCmd);
short FindAFreeChannel( MADDriverRec *intDriver);
OSErr	ASIOSndClose( MADDriverRec *inMADDriver);
OSErr	InitASIOManager( MADDriverRec *inMADDriver, long init);

// Effects.c

void BL32( void *msg_buf);
void BL16( void *msg_buf);

void parse_slidevol(Channel *ch, Byte Arg);
void ConvertTo64Rows( MADMusic *music);
//void						BL32( void *msg_buf);
//void						BL16( void *msg_buf);

struct __MADDriverRec
{
	/**********************/
	/** Public variables **/
	/**********************/
	
	MADDriverSettings		DriverSettings;									// Driver SetUp -- READ ONLY --
	
	/**  Current music in memory, loaded with RLoadMusic() by example **/
	
	MADMusic				*curMusic;										// Current music played by this driver, it can be 0L !!!
	MADLibrary				*lib;
	
	/**  Drivers variables **/
	
	Channel					chan[ MAXTRACK];								// Current driver channels -- READ ONLY --
	Boolean					musicEnd;										// Is music playing finished? -- READ ONLY --
	short					Tube[ MAXTRACK];								// Used in 'Tracks View' Window - View menu 
	short					PartitionReader;								// Current position in pattern (0...999)
	short					Pat;											// Current ID Pattern, see 'Patterns list'
	short					PL;												// Current position in partition, see 'Partition list'
	long					VolGlobal;										// Global SOFTWARE volume (This is NOT Mac hardware volume!) from 0 to 64
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
	long					OscilloWaveSize;								// Size of previous buffer
	
	/** Private variables - Not documented **/
	/* DO NOT MODIFY OR USE these variables */
	
	long					BytesToRemoveAtEnd, MIN_PITCH, MAX_PITCH, MOD_MIN_PITCH, MOD_MAX_PITCH, ASCBUFFERReal;
	short					smallcounter, trackDiv;
	long					FREQBASE;
	short					InstruActif[ MAXINSTRU];
	Ptr						SysHeapPtr, IntDataPtr;
	Ptr						OsciDrawPtr[ MAXDRAW];
	Boolean					newData[ MAXDRAW];
	Boolean					useOsciBuffers;
	short					curDrawPtr;
	unsigned long			curTime;
	Boolean					XMLinear, MODMode, JumpToNextPattern, endPattern, MADPlay;
	long					ASCBUFFER;
	long					BufSize;
	long					VSYNC, BufCounter, BytesToGenerate;
	short					vibrato_table[ 64];
	short					SendMIDIClockData;	//gOutNodeRefNum, MIDIPortRefNum
	short					InstuNoOld[ MAXTRACK];
	short					NoteOld[ MAXTRACK];
	short					VelocityOld[ MAXTRACK];
	Boolean					TrackLineReading[ MAXTRACK], TrackReading[ MAXTRACK], wasReading;
	Ptr						OverShoot;
	long					*DASCBuffer;//, *DASCBufferOut;
	long					*DASCEffectBuffer[ MAXCHANEFFECT];
	long					EffectBufferID[ MAXCHANEFFECT];
	long					EffectBufferRealID[ MAXCHANEFFECT];
	short					*DASCBuffer8;//, *DASCBuffer8Out;
	double					*Filter, *fData;
	long					MDelay;
	long					RDelay;
	Ptr						ReverbPtr;
	short					PatDelay;
	short					lastChannelUsed[ MAXTRACK];
	long					MultiChanNo, globPan;
	Boolean					currentlyExporting;
	Boolean					thisExport;
	Boolean					OneMoreBeforeEnd;
	Boolean					clipL, clipR;
	long					levelL, levelR;
	long					curCenterL, curCenterR;
	
	
#ifdef _MAC_H
	VSTEffect				*masterVST[ 10];
	VSTEffect				*chanVST[ MAXTRACK][ 4];
	//PPSndDoubleBufferHeader 	TheHeader;
#endif
	
};
	

#ifdef __cplusplus
}
#endif
#endif
