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

#ifndef __RDRIVERH__
#define __RDRIVERH__

#ifndef __MADI__
#include "MAD.h"
#endif

////////////////////////////////////////////////

#ifdef _MAC_H
#ifndef __SOUND__
#include <Carbon/Carbon.h>
#endif
#include <CoreFoundation/CFPlugInCOM.h>
#endif

////////////////////////////////////////////////

#ifdef WIN32
#ifndef __DSOUND_INCLUDED__
#include <mmreg.h>
#include <DSound.h>
#endif
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

//The following aren't defined in Mac headers, but are used
enum{
#if !defined(_MAC_H) || defined(__LP64__)
	rate48khz	= (UnsignedFixed)0xBB800000,
	rate44khz	= (Fixed)0xAC440000,
	rate32khz	= (Fixed)0x7D000000,
	rate22050hz	= (Fixed)0x56220000,
	rate22khz	= (Fixed)0x56EE8BA3,
	rate16khz	= (Fixed)0x3E800000,
	rate11khz	= (Fixed)0x2B7745D1,
	rate11025hz	= (Fixed)0x2B110000,
	rate8khz	= (Fixed)0x1F400000,
#endif
	rate5khz	= (UnsignedFixed)0x15BB9B5CUL,
	rate2khz	= (UnsignedFixed)0x08000000UL
};

////////////////////////////////////////////////

#if PRAGMA_STRUCT_ALIGN
#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
#pragma pack(2)
#endif

////////////////////////////////////////////////

/********************						***********************/
/*** 			   		 Define List							***/
/********************						***********************/

#define NUMBER_NOTES		96
#define NOFINETUNE			8363
#define MIN_VOLUME			0
#define MAX_VOLUME			64
#define MAX_CHANVOL			128
#define MAX_PANNING			64
#define MAXDRAW				10
//#define MAXTRACK			32
//#define MAXINSTRU			64
#define MAXSAMPLE			64L
#define MAXPATTERN			200
#define MAXPATTERNSIZE		900
#define MAX_ARP 			3
#define MAXPLUG				40
#define EQPACKET			512
#define MAXCHANEFFECT		20

/********************						***********************/
/*** 			  		  Error messages 						***/
/********************						***********************/

enum
{
	MADNeedMemory 					= -1,
	MADReadingErr					= -2,
	MADIncompatibleFile				= -3,
	MADLibraryNotInitialized		= -4,
	MADParametersErr				= -5,
	MADUnknowErr					= -6,
	MADSoundManagerErr				= -7,
	MADOrderNotImplemented			= -8,
	MADFileNotSupportedByThisPlug	= -9,
	MADCannotFindPlug				= -10,
	MADMusicHasNoDriver				= -11,
	MADDriverHasNoMusic				= -12,
	MADUnknownErr					= MADUnknowErr
};

/********************						***********************/
/*** 			   Channel structure definition					***/
/********************						***********************/

typedef struct Channel
{
		long		ID;					// Channel ID - 0 to MAXTRACK
		long		TrackID;			// TrackID - 0 to MAXTRACK (Used in multiChannel mode)
		
		Ptr			begPtr;				// Sample Data Ptr - Beginning of data
		Ptr			maxPtr;				// Sample Data Ptr - End of data
		Ptr			curPtr;				// Sample Data Ptr - Current position
		long		sizePtr;			// Sample Size in bytes
		
		long		amp;				// Sample amplitude: 8 or 16 bits
		
		long		loopBeg;			// Loop Beginning
		long		loopSize;			// Loop Size
		
		long		ins;				// Current Instrument ID
		long		insOld;				// Previous Instrument ID played on this channel
		long		samp;				// Current Sample ID
		
		long		fineTune;			// Finetune
		
		long		note;				// Note
		long		noteOld;			// Previous note played on this channel
		long		relNoteOld;			// Previous realnote
		
		long 		period;				// Current period
		long		periodOld;			// Previous period played on this channel
		
		long		vol;				// Channel vol (0 to 64)
		long		pann;				// Channel pan (0 to 64)
		
		long 		cmd;				// Command
		Byte		arg;				// Argument of command
		Byte		volcmd;				// Volume Command
		
		long 		arp[ MAX_ARP];		// Used for arpeggio command
		long 		arpindex;			// Used for arpeggio command
		Boolean		arpUse;
		
		char		viboffset;			// Used for vibrato command
		long 		vibdepth;			// Used for vibrato command
		long 		vibrate;			// Used for vibrato command
		long		vibtype;			// Used for vibrato command
		
		long 		slide;				// Used for slideUp and slideDown command
		
		long 		pitchgoal;			// Used for portamento command
		long 		pitchrate;			// Used for portamento command
		
		long 		volumerate;			// Used for slideVolume command
		
		long		oldArg[ 16];
		long		oldVibrate;
		long		oldVibdepth;
		
		long		eventTime;
		
		Ptr			samplePtr;			// Used internaly by MADPlaySoundData & MADPlaySndHandle
		
		/**/
		
		Boolean		KeyOn;
		short		a;
		short		b;
		float		p;
		long		volEnv;
		long		nextvolEnv;
		long		volEnvInter;
		
		long		volFade;
		long		nextvolFade;
		
		short		aa;
		short		bb;
		float		pp;
		long		pannEnv;
		long		nextpannEnv;
//		long		pannEnvInter;
		
		Boolean		volEnvActive, pannEnvActive;
		
		short		aaa;
		short		bbb;
		float		ppp;
		long		panPitch;
		
		long		lAC;
		
		Ptr			prevPtr;
		long		lastWordL, curLastWordL;
		long		lastWordR, curLastWordR;
		long		curLevelL, curLevelR;
		
		Boolean		LevelDirectionL, LevelDirectionR, RemoverWorking;
		
		long		prevVol0;
		long		prevVol1;
		
		/**/
		
		Boolean		GEffect;
		short		GPat, GReader;
		
		/**/
		
		Boolean		stereo;
		
		Byte		loopType;
		Boolean		pingpong;
		
		long		preOff;
		char		preVal, preVal2;
		char		preValR, preVal2R;
		
		short		spreVal, spreVal2;
		short		spreValR, spreVal2R;
		long		TICKREMOVESIZE;
		
		
		Boolean		PanningE8;
		short		trig;
		short		PatternLoopE6, PatternLoopE6Count, PatternLoopE6ID;
		
		long		TimeCounter;
} Channel;

/********************						***********************/
/*** 		Music description - used in Import/Export filter	***/
/********************						***********************/

typedef struct MADMusic
{
	MADSpec					*header;								// Music Header - See 'MAD.h'
	PatData					*partition[ MAXPATTERN];				// Patterns
	InstrData				*fid;									// Instruments
	sData					**sample;								// Samples
	FXSets					*sets;									// FXSettings
	Boolean					musicUnderModification;					// Tell the driver to NOT access music data
	Str255					musicFileName;
	Boolean					hasChanged;
	long					position, fullTime;
	OSType					originalFormat;
	
} MADMusic;

/********************						***********************/
/*** 			     Driver Settings definition					***/
/********************						***********************/

enum
{
	oldASCSoundDriver = 1,			// MAC ONLY,	// NOT SUPPORTED anymore
	oldAWACSoundDriver,				// MAC ONLY		// NOT SUPPORTED anymore
	MIDISoundDriver,				// MAC ONLY
	SoundManagerDriver,				// MAC ONLY You should use only SoundManagerDriver for full compatibility !
	QK25SoundDriver,				// MAC ONLY
	DigiDesignSoundDriver,			// MAC ONLY
	BeOSSoundDriver,				// BE ONLY when using with BeOS compatible systems ! - NOT FOR MAC
	DirectSound95NT,				// WINDOWS 95/NT ONLY when using with PC compatible systems ! - NOT FOR MAC
	Wave95NT,						// WINDOWS 95/NT ONLY when using with PC compatible systems ! - NOT FOR MAC
	CoreAudioDriver,				// OSX ONLY Core Audio driver
	ALSADriver,						// LINUX ONLY ALSA driver
	OSSDriver,						// Open Sound System. Most Unices (NOT OS X) including Linux
	ESDDriver,						// ESound Driver. available on most UNIX Systems
	ASIOSoundManager,				// ASIO Sound Driver by Steinberg
	NoHardwareDriver				// NO HARDWARE CONNECTION, will not produce any sound

};

enum
{
	oldMonoOutPut = 1,				// NOT SUPPORTED anymore
	oldStereoOutPut,				// NOT SUPPORTED anymore
	DeluxeStereoOutPut,				// USE THIS ONE ONLY !!!!!!!!!!!!!!
	PolyPhonic,						// Do NOT use it ! Standard hardware doesn't support it !
	
	StereoOutPut = oldStereoOutPut,
	MonoOutPut = oldMonoOutPut
};

typedef struct MADDriverSettings
{
	short					numChn;								// Active tracks from 2 to 32, automatically setup when a new music is loaded
	short					outPutBits;							// 8 or 16 Bits TODO: 24 Bits
	UnsignedFixed			outPutRate;							// Fixed number, by example : rate44khz, rate22050hz, rate22khz, rate11khz, rate11025hz
	short					outPutMode;							// Now, only DeluxeStereoOutPut is available !
	short					driverMode;							// MIDISoundDriver, SoundManagerDriver, BeOSSoundDriver, DirectSound95NT or Wave95NT
//	Boolean					antiAliasing;						// NOT USED anymore
	Boolean					repeatMusic;						// If music finished, repeat it or stop.
	Boolean					sysMemory;							// Allocate memory in Application Heap (false) or in System Heap (true)
//	Boolean					Interpolation;						// NOT USED anymore
//	Boolean					MicroDelay;							// NOT USED anymore
	long					MicroDelaySize;						// Micro delay duration (in ms, max 1 sec = 1000 ms, min = 0 ms)
	Boolean					surround;							// Surround effect active? true/false
	Boolean					Reverb;								// Reverb effect active? true/false
	long					ReverbSize;							// Reverb delay duration (in ms, min = 25 ms, max 1 sec = 1000 ms)
	long					ReverbStrength;						// Reverb strength in % (0 <-> 70)
	Boolean					TickRemover;						// Remove volume/sample/loop ticks.
	long					oversampling;						// OverSampling value, 1 = normal; works ONLY on 64bits processor (PowerPC)
} MADDriverSettings;

/******************************************************************/
//******************* MUSICS IMPORT/EXPORT PLUGS  *****************/
//
//	Protocol Version 2.3
//
//	To use with PlayerPRO for CodeWarrior
//
//	Your main function have to be in this form:
//	OSErr main( 	OSType order,
//					Ptr AlienFileName,
//					MADMusic *MadFile,
//					PPInfoRec *info,
//					MADDriverSettings *DriverParam);
//
//	Actual plug have to support these orders:
//
//	order: 'TEST':	check the AlienFile to see if your Plug really supports it.
//	order: 'IMPT':	convert the AlienFile into a MADMusic. You have to allocate MADMusic.
//	order: 'INFO':	Fill PPInfoRec structure.
//	order: 'EXPT':	Convert the MADMusic into AlienFile. You have to create the AlienFile.
//					Don't delete the MADMusic Structure after conversion !!
//
//	An IMPORT plug have to support these orders: 'TEST', 'IMPT', 'INFO'
//	An EXPORT plug have to support these orders: 'EXPT'
// 	An IMPORT/EXPORT plug have to support these orders: 'TEST', 'IMPT', 'INFO', 'EXPT'
//
//	About Resources:
//
//	Your Plug should have: Creator: 'SNPL', Type: 'IMPL' - MAC ONLY
//
//	Your Plug have to have these resources - MAC ONLY:
//
//	- One segment CODE 1000 with 68k Code
//	- One segment PPCC 1000 with PPC Code (OPTIONAL: if PlayerPRO in PPC cannot find it, it will use the CODE 1000 resource)
//	- One STR# resource :
//
//		1 string: which kind of files your plug support (OSType value!!! 4 char) By example: 'STrk', '669 ', etc...
//		2 string: what does your Plug: EXPL : only Export files, IMPL : only Import Files, EXIM : import AND export.
//		3 string: string that will be used in Import and Export menu of PlayerPRO
//		4 string: Copyright string of this plug.
//
/********************						***********************/

typedef struct PPInfoRec
{
	char		internalFileName[ 60];
	char		formatDescription[ 60];
	
	long		totalPatterns;
	long		partitionLength;
	
	short		totalTracks;
	short		totalInstruments;
	
	OSType		signature;
	
	long		fileSize;
	
} PPInfoRec;


/********************						***********************/
/*** 			Informations about Plugs: ThePlug[]				***/
/********************						***********************/

#ifdef _MAC_H

enum {
  PPdbBufferReady                 = 0x00000001, /*double buffer is filled*/
  PPdbLastBuffer                  = 0x00000004 /*last double buffer to play*/
};

typedef CALLBACK_API( OSErr , PPSndDoubleBackProcPtr )(void);
typedef STACK_UPP_TYPE(PPSndDoubleBackProcPtr)                    PPSndDoubleBackUPP;

typedef struct PPSndDoubleBuffer {
  long                dbNumFrames;
  long                dbFlags;
  long                dbUserInfo[2];
  SInt8               dbSoundData[1];
} PPSndDoubleBuffer;
typedef PPSndDoubleBuffer *               PPSndDoubleBufferPtr;

typedef struct PPSndDoubleBufferHeader {
  short               dbhNumChannels;
  short               dbhSampleSize;
  short               dbhCompressionID;
  short               dbhPacketSize;
  UnsignedFixed       dbhSampleRate;
  PPSndDoubleBufferPtr  dbhBufferPtr[2];
  PPSndDoubleBackUPP    dbhDoubleBack;
} PPSndDoubleBufferHeader;
typedef PPSndDoubleBufferHeader *         PPSndDoubleBufferHeaderPtr;


typedef struct PPSndDoubleBufferHeader2 {
  short               dbhNumChannels;
  short               dbhSampleSize;
  short               dbhCompressionID;
  short               dbhPacketSize;
  UnsignedFixed       dbhSampleRate;
  PPSndDoubleBufferPtr  dbhBufferPtr[2];
  PPSndDoubleBackUPP    dbhDoubleBack;
  OSType              dbhFormat;
} PPSndDoubleBufferHeader2;
typedef PPSndDoubleBufferHeader2 *        PPSndDoubleBufferHeader2Ptr;

#define kPlayerPROModFormatTypeID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x84, 0xF8, 0x01, 0x09, 0x28, 0x85, 0x4E, 0x01, 0x8F, 0xFA, 0x88, 0xAC, 0x75, 0xF3, 0xE0, 0x33))
//84F80109-2885-4E01-8FFA-88AC75F3E033

#define kPlayerPROModFormatInterfaceID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorDefault, 0x94, 0x15, 0xE3, 0x57, 0x0E, 0xD2, 0x4F, 0x9F, 0x9C, 0xA1, 0xB7, 0x28, 0x0C, 0x27, 0xF5, 0x9B))
//9415E357-0ED2-4F9F-9CA1-B7280C27F59B

typedef struct _MADFileFormatPlugin {
    IUNKNOWN_C_GUTS;
	OSErr (STDMETHODCALLTYPE *ThePlugMain)(OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);
} MADFileFormatPlugin;

typedef struct PlugInfo
{
	MADFileFormatPlugin **IOPlug;								// Plug CODE
	CFStringRef	MenuName;										// Plug name
	CFStringRef	AuthorString;									// Plug author
	CFBundleRef	file;											// Location of plug file
	char		type[ 5];										// OSType of file support. Kept for legacy reasons
	CFArrayRef	UTItypes;										// CFStrings of supported UTIs
	OSType		mode;											// Mode support : Import +/ Export
} PlugInfo;
#endif

#ifdef WIN32
#include <windows.h>
typedef OSErr (*PLUGDLLFUNC) ( OSType , Ptr , MADMusic* , PPInfoRec *, MADDriverSettings *);
struct PlugInfo
{
	HANDLE			hLibrary;
	PLUGDLLFUNC		IOPlug;										// Plug CODE
	char			MenuName[ 65];								// Plug name
	char			AuthorString[ 65];							// Plug author
	char			file[ 255];									// Location of plug file
	char			type[ 5];									// OSType of file support
	OSType			mode;										// Mode support : Import +/ Export
};
typedef struct PlugInfo PlugInfo;
#endif

#ifdef __UNIX__
#include <dlfcn.h>
typedef OSErr (*MADPLUGFUNC) ( OSType , Ptr , MADMusic* , PPInfoRec *, MADDriverSettings *);
struct PlugInfo
{
	void*			hLibrary;
	MADPLUGFUNC		IOPlug;										// Plug CODE
	char			MenuName[ 65];								// Plug name
	char			AuthorString[ 65];							// Plug author
	char			file[ PATH_MAX];							// Location of plug file
	char			type[ 5];									// OSType of file support
	OSType			mode;										// Mode support : Import +/ Export
};
typedef struct PlugInfo PlugInfo;
#endif


/********************						***********************/
/*** 		Global structure : PlayerPRO variables				***/
/********************						***********************/

struct MADLibrary
{
	OSType					IDType;								// IDType = 'MADD' -- READ ONLY --
	Boolean					sysMemory;
	long					mytab[ 12];
	
	/** Plugs Import/Export variables **/
	
	PlugInfo 				*ThePlug;							// Pointers on plugs code & infos
	short					TotalPlug;							// no of Plugs in pointer ThePlug

};
typedef struct MADLibrary MADLibrary;

#define callback

typedef struct AEffect AEffect;

struct AEffect
{
	long magic;
	long (callback *dispatcher)(AEffect *effect, long opCode, long index, long value,
		void *ptr, float opt);
	void (callback *process)(AEffect *effect, float **inputs, float **outputs, long sampleframes);
	void (callback *setParameter)(AEffect *effect, long index, float parameter);
	float (callback *getParameter)(AEffect *effect, long index);

	long numPrograms;
	long numParams;
	long numInputs;
	long numOutputs;
	long flags;
	long resvd1;
	long resvd2;
	long initialDelay;
	long realQualities;
	long offQualities;
	float ioRatio;
	void *object;
	void *user;
	long uniqueID;
	long version;
	void (callback *processReplacing)(AEffect *effect, float **inputs, float **outputs, long sampleframes);
	char future[60];
};

typedef	long (*audioMasterCallback)(AEffect *effect, long opcode, long index,
		long value, void *ptr, float opt);
	
	typedef AEffect *(*VSTPlugInPtr) (audioMasterCallback  cb);

#ifdef _MAC_H
typedef struct
{
	AEffect				*ce[ 2];
	short				id;
	Str63				name;
	Boolean				Active;
	CFragConnectionID	connID; //TODO: use something more 64-bit friendly
	VSTPlugInPtr		vstMain;
	Boolean				ProcessReplacingNotAvailable;

}	VSTEffect;
#endif

typedef struct MADDriverRec
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
	SndChannelPtr 			MusicChannelPP;									// The SndChannelPtr to apply SndDoCommand, etc.
																			// ONLY available if you are using MAC SoundManager driver
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
	PPSndDoubleBufferHeader 	TheHeader;
#endif

} MADDriverRec;

/********************						***********************/
/*** 					   EFFECTS ID							***/
/********************						***********************/

enum {
		arpeggioE 		= 0,	//	0x00
		downslideE 		= 1,	//	0x01
		upslideE 		= 2,	//	0x02
		portamentoE 	= 3,	//	0x03
		vibratoE 		= 4,	//	0x04
		portaslideE 	= 5,	//	0x05
		vibratoslideE	= 6,	//	0x06
		nothingE 		= 7,	//	0x07
		panningE		= 8,	//	0x08
		offsetE 		= 9,	//	0x09
		slidevolE 		= 10,	//	0x0A
		fastskipE 		= 11,	//	0x0B
		volumeE 		= 12,	//	0x0C
		skipE 			= 13,	//	0x0D
		extendedE 		= 14,	//	0x0E
		speedE 			= 15,	//	0x0F
		noteoffE 		= 16,	//	0x0G
		LoopE			= 17,	// 	0x0L
		NOffSetE		= 18	//  0x0O
	};

/********************						***********************/
/*** 					   FUNCTIONS							***/
/********************						***********************/


#ifdef __cplusplus
extern "C" {
#endif

void MyDebugStr( short, char*, char*);								// Internal Debugger function, NORMALLY it is never called, only when FATAL error
																	// You NEED to provide this function, see examples!

MADLibrary* MADGetMADLibraryPtr();									// Get MADDriver structure pointer.

OSErr	MADInitLibrary( FSSpec *PlugsFolderName, Boolean sysMemory, MADLibrary **MADLib);	// Library initialisation, you have to CALL this function if you want to use other functions & variables
OSErr	MADDisposeLibrary( MADLibrary *MADLib);						// Close Library, close music, close driver, free all memory

void	MADGetBestDriver( MADDriverSettings	*DriverInitParam);		// Found and identify the current Mac sound hardware and fill DriverInitParam
OSErr	MADCreateDriver( MADDriverSettings	*DriverInitParam, MADLibrary *MADLib, MADDriverRec** returnDriver);		// Music Driver initialization and memory allocation
OSErr	MADDisposeDriver( MADDriverRec *MDriver);											// Dispose the music driver, use it after RInitMusic()

OSErr	MADChangeDriverSettings( MADDriverSettings	*DriverInitParam, MADDriverRec** returnDriver);

OSErr	MADStartDriver( MADDriverRec *MDriver);										// NEW - Activate the sound generating procedure (interruption)
OSErr	MADStopDriver( MADDriverRec *MDriver);										// NEW - DESActivate the sound generating procedure (interruption)

OSErr	MADPlayMusic( MADDriverRec *MDriver);										// NEW - Read and play current music in memory - Call MADStartInterruption BEFORE
OSErr	MADStopMusic( MADDriverRec *MDriver);										// NEW - Stop reading current music in memory, Use MADCleanDriver to stop sounds
void	MADCleanDriver( MADDriverRec *intDriver);									// Clean the driver : stop playing sounds

OSErr	MADReset( MADDriverRec *MDriver);											// Reset the current music at the start position
OSErr	MADGetMusicStatus( MADDriverRec *MDriver, long *fullTime, long *curTime);			// Get informations about music position and duration, IN 1/60th SECS !! NOT IN SECS ANYMORE !!!!!!!
OSErr	MADSetMusicStatus( MADDriverRec *MDriver, long minV, long maxV, long curV);			// Change position of current music, by example MADSetMusicStatus( 0, 100, 50) = go to the middle of the music

OSErr	MADSetHardwareVolume( long);								// 0...64, Mac HARDWARE volume, see MADDriver->VolGlobal for SOFTWARE volume
long	MADGetHardwareVolume( void);										// Return HARDWARE volume, see MADDriver->VolGlobal for SOFTWARE volume

OSErr	MADAttachDriverToMusic( MADDriverRec *driver, MADMusic *music, unsigned char*);

OSErr	MADLoadMusicRsrc( MADMusic **music, OSType IDName, short IDNo);				// MAD ONLY - Load a MAD Rsrc into memory
OSErr	MADLoadMusicPtr( MADMusic **music, Ptr myPtr);								// MAD ONLY - Load a MAD Ptr into memory, you can DisposPtr your Ptr after this call

OSErr	MADLoadMusicFilePString( MADLibrary *, MADMusic **music, char *type, Str255 fName);			// Load a music file with plugs
OSErr	MADLoadMusicFileCString( MADLibrary *, MADMusic **music, char *type, Ptr fName);			// Load a music file with plugs
OSErr	MADLoadMusicFSpFile( MADLibrary *, MADMusic **music, char *type, FSSpec *theSpec);			// Load a music file with plugs

OSErr	MADMusicIdentifyPString( MADLibrary *, char *type, Str255 pName);			// Identify what kind of music format is pName file.
OSErr	MADMusicIdentifyCString( MADLibrary *, char *type, Ptr cName);			// Identify what kind of music format is cName file.
OSErr	MADMusicIdentifyFSp( MADLibrary *, char *type, FSSpec *theSpec);			// Identify what kind of music format is theSpec file.

Boolean	MADPlugAvailable( MADLibrary *, char *type);								// Is plug 'type' available?

OSErr	MADDisposeMusic( MADMusic **, MADDriverRec *MDriver);								// Dispose the current music, use it after RLoadMusic(), RLoadMusicRsrc(), RInstallMADF()

void	MADChangeTracks( MADDriverRec *MDriver, short);				// Change current tracks number of the music driver
Cmd*	GetMADCommand(	short		position,						// Extract a Command from a PatData structure
						short		channel,
						PatData*	aPatData);

OSErr	MADPlaySndHandle( 	MADDriverRec *MDriver,
							Handle sound,							// Handle to a 'snd ' handle, by ex: GetResource('snd ', 128);
							long chan,								// channel ID on which to play sound
							long note);								// note: 0 to NUMBER_NOTES or 0xFF: play sound at his normal sampleRate Khz

OSErr	MADPlaySoundData(	MADDriverRec *MDriver,
							Ptr				soundPtr,				// Sound Pointer to data
							long			size,					// Sound size in bytes
							long			channel,				// channel ID on which to play sound
							long			note,					// note: 0 to NUMBER_NOTES or 0xFF: play sound at 22 Khz
							long			amplitude,				// 8 or 16 bits
							long			loopBeg,				// loop beginning
							long			loopSize,				// loop size in bytes
							unsigned long	rate,					// sample rate of the sound data, by ex: rate22khz
							Boolean			stereo);				// sample is in stereo or in mono?
							
OSErr	MADPlaySoundDataSYNC(MADDriverRec *MDriver,
							Ptr				soundPtr,				// Sound Pointer to data
							long			size,					// Sound size in bytes
							long			channel,				// channel ID on which to play sound
							long			note,					// note: 0 to NUMBER_NOTES or 0xFF: play sound at 22 Khz
							long			amplitude,				// 8 or 16 bits
							long			loopBeg,				// loop beginning
							long			loopSize,				// loop size in bytes
							unsigned long	rate,					// sample rate of the sound data, by ex: rate22khz
							Boolean			stereo);				// sample is in stereo or in mono?

//Ptr MADNewPtr( long size, MADLibrary* init);
//Ptr MADNewPtrClear( long size, MADLibrary* init);
//Since we don't use MADLibrary any more, redefining these terms
#define MADNewPtr(size, madlib) NewPtr(size)
#define MADNewPtrClear(size, madlib) NewPtrClear(size)
#define MADPlugNewPtr(size, madlib) NewPtr(size)
#define MADPlugNewPtrClear(size, madlib) NewPtrClear(size)
	
void MyDebugStr(short, Ptr, Ptr);									// Called when a fatal error occurs.... Normally, NEVER !

#ifdef __cplusplus
}
#endif

#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
#pragma pack()
#endif
#endif
