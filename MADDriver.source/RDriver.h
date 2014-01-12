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
#include <limits.h>

////////////////////////////////////////////////

#pragma pack(push, 2)

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
#define MAXSAMPLE			64
#define MAXPATTERN			200
#define MAXPATTERNSIZE		900
#define MAX_ARP 			3
#define MAXPLUG				40
#define EQPACKET			512
#define MAXCHANEFFECT		20

/********************						***********************/
/*** 			  		  Error messages 						***/
/********************						***********************/

enum MADErrors
{
	MADIsRunningSheet				= 10,
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
	MADSoundSystemUnavailable		= -13,
	MADWritingErr					= -14,
	MADUnknownErr					= MADUnknowErr
};

/********************						***********************/
/*** 			   Channel structure definition					***/
/********************						***********************/

typedef struct Channel
{
	int		ID;					// Channel ID - 0 to MAXTRACK
	int		TrackID;			// TrackID - 0 to MAXTRACK (Used in multiChannel mode)
	
	char	*begPtr;			// Sample Data Ptr - Beginning of data
	char	*maxPtr;			// Sample Data Ptr - End of data
	char	*curPtr;			// Sample Data Ptr - Current position
	size_t	sizePtr;			// Sample Size in bytes
	
	int		amp;				// Sample amplitude: 8 or 16 bits
	
	size_t	loopBeg;			// Loop Beginning
	size_t	loopSize;			// Loop Size
	
	int		ins;				// Current Instrument ID
	int		insOld;				// Previous Instrument ID played on this channel
	int		samp;				// Current Sample ID
	
	int		fineTune;			// Finetune
	
	int		note;				// Note
	int		noteOld;			// Previous note played on this channel
	int		relNoteOld;			// Previous realnote
	
	int		period;				// Current period
	int		periodOld;			// Previous period played on this channel
	
	int		vol;				// Channel vol (0 to 64)
	int		pann;				// Channel pan (0 to 64)
	
	int		cmd;				// Command
	Byte	arg;				// Argument of command
	Byte	volcmd;				// Volume Command
	
	SInt32	arp[MAX_ARP];		// Used for arpeggio command
	int		arpindex;			// Used for arpeggio command
	Boolean	arpUse;
	
	char	viboffset;			// Used for vibrato command
	int		vibdepth;			// Used for vibrato command
	int		vibrate;			// Used for vibrato command
	int		vibtype;			// Used for vibrato command
	
	int		slide;				// Used for slideUp and slideDown command
	
	int 	pitchgoal;			// Used for portamento command
	int 	pitchrate;			// Used for portamento command
	
	int 	volumerate;			// Used for slideVolume command
	
	SInt32	oldArg[16];
	int		oldVibrate;
	int		oldVibdepth;
	
	int		eventTime;
	
	char	*samplePtr;			// Used internaly by MADPlaySoundData
	
	/**/
	
	Boolean	KeyOn;
	short	a;
	short	b;
	float	p;
	int		volEnv;
	int		nextvolEnv;
	int		volEnvInter;
	
	int		volFade;
	int		nextvolFade;
	
	short	aa;
	short	bb;
	float	pp;
	int		pannEnv;
	int		nextpannEnv;
	//long	pannEnvInter;
	
	Boolean	volEnvActive, pannEnvActive;
	
	short	aaa;
	short	bbb;
	float	ppp;
	int		panPitch;
	
	int		lAC;
	
	char	*prevPtr;
	int		lastWordL, curLastWordL;
	int		lastWordR, curLastWordR;
	int		curLevelL, curLevelR;
	
	Boolean	LevelDirectionL, LevelDirectionR, RemoverWorking;
	
	int		prevVol0;
	int		prevVol1;
	
	/**/
	
	Boolean	GEffect;
	short	GPat, GReader;
	
	/**/
	
	Boolean	stereo;
	
	Byte	loopType;
	Boolean	pingpong;
	
	int		preOff;
	char	preVal, preVal2;
	char	preValR, preVal2R;
	
	short	spreVal, spreVal2;
	short	spreValR, spreVal2R;
	int		TICKREMOVESIZE;
	
	
	Boolean	PanningE8;
	short	trig;
	short	PatternLoopE6, PatternLoopE6Count, PatternLoopE6ID;
	
	int		TimeCounter;
} Channel;

/********************						***********************/
/*** 		Music description - used in Import/Export filter	***/
/********************						***********************/

typedef struct MADMusic
{
	int			position, fullTime;
	MADSpec		*header;					// Music Header - See 'MAD.h'
	PatData		*partition[MAXPATTERN];		// Patterns
	InstrData	*fid;						// Instruments
	sData		**sample;					// Samples
	FXSets		*sets;						// FXSettings
	OSType		originalFormat;
	Str255		musicFileName;
	Boolean		musicUnderModification;		// Tell the driver to NOT access music data
	Boolean		hasChanged;
} MADMusic;

/********************						***********************/
/*** 			     Driver Settings definition					***/
/********************						***********************/

enum MADSoundOutput
{
	CoreAudioDriver = 1,		// OSX ONLY Core Audio driver
	DirectSound95NT,			// WINDOWS ONLY
	Wave95NT,					// WINDOWS ONLY
	PortAudioDriver,			// Can be present on multiple OSes
	PulseAudioDriver,			// Mostly used by Linux/Unix
	ESDDriver,					// ESound Driver. available on most UNIX Systems
	BeOSSoundDriver,			// BeOS/Haiku ONLY
	MIDISoundDriver,			// Not yet available
	ASIOSoundManager,			// ASIO Sound Driver by Steinberg //NOT Available
	NoHardwareDriver = SHRT_MAX	// NO HARDWARE CONNECTION, will not produce any sound
};

//Used for MADSoundDriverList()
enum MADSoundDriverAvailable
{
	MIDISoundDriverBit	= 1 << MIDISoundDriver,
	BeOSSoundDriverBit	= 1 << BeOSSoundDriver,
	DirectSound95NTBit	= 1 << DirectSound95NT,
	Wave95NTBit			= 1 << Wave95NT,
	CoreAudioDriverBit	= 1 << CoreAudioDriver,
	ESDDriverBit		= 1 << ESDDriver,
	PortAudioDriverBit	= 1 << PortAudioDriver,
	PulseAudioDriverBit	= 1 << PulseAudioDriver,
	ASIOSoundManagerBit	= 1 << ASIOSoundManager
};

enum OutputChannels
{
	oldMonoOutPut = 1,	// NOT SUPPORTED anymore
	oldStereoOutPut,	// NOT SUPPORTED anymore
	DeluxeStereoOutPut,	// USE THIS ONE ONLY !!!!!!!!!!!!!!
	PolyPhonic,			// Do NOT use it ! Standard hardware doesn't support it !
	
	StereoOutPut = oldStereoOutPut,
	MonoOutPut = oldMonoOutPut
};

typedef struct MADDriverSettings
{
	short			numChn;				// Active tracks from 2 to 32, automatically setup when a new music is loaded
	short			outPutBits;			// 8 or 16 Bits TODO: 24 Bits
	short			outPutMode;			// Now, only DeluxeStereoOutPut is available !
	short			driverMode;			// MIDISoundDriver, SoundManagerDriver, BeOSSoundDriver, DirectSound95NT or Wave95NT
	unsigned int	outPutRate;			// Integer of audio sample rate
	int				MicroDelaySize;		// Micro delay duration (in ms, max 1 sec = 1000 ms, min = 0 ms)
	int				ReverbSize;			// Reverb delay duration (in ms, min = 25 ms, max 1 sec = 1000 ms)
	int				ReverbStrength;		// Reverb strength in % (0 <-> 70)
	int				oversampling;		// OverSampling value, 1 = normal; works ONLY on 64bits processor (PowerPC)
	Boolean			TickRemover;		// Remove volume/sample/loop ticks.
	Boolean			surround;			// Surround effect active? true/false
	Boolean			Reverb;				// Reverb effect active? true/false
	Boolean			repeatMusic;		// If music finished, repeat it or stop.
} MADDriverSettings;

/******************************************************************/
//******************* MUSICS IMPORT/EXPORT PLUGS  *****************/
//
//	Protocol Version 2.3
//
//	To use with PlayerPRO for CodeWarrior
//
//	Your main function have to be in this form:
//	OSErr PPImpExpMain(	OSType order,
//						char *AlienFileName,
//						MADMusic *MadFile,
//						PPInfoRec *info,
//						MADDriverSettings *DriverParam);
//
//	Actual plug have to support these orders:
//
//	order: 'TEST':	check the AlienFile to see if your Plug really supports it.
//	order: 'IMPL':	convert the AlienFile into a MADMusic. You have to allocate MADMusic.
//	order: 'INFO':	Fill PPInfoRec structure.
//	order: 'EXPL':	Convert the MADMusic into AlienFile. You have to create the AlienFile.
//					Don't delete the MADMusic Structure after conversion !!
//
//	An IMPORT plug have to support these orders: 'TEST', 'IMPL', 'INFO'
//	An EXPORT plug have to support these orders: 'EXPL'
// 	An IMPORT/EXPORT plug have to support these orders: 'TEST', 'IMPL', 'INFO', 'EXPL'
//
//
/********************						***********************/

typedef struct PPInfoRec
{
	int		totalPatterns;
	int		partitionLength;
	
	long	fileSize;
	
	OSType	signature;
	
	short	totalTracks;
	short	totalInstruments;
	
	char	internalFileName[60];
	char	formatDescription[60];
} PPInfoRec;


/********************						***********************/
/*** 			Informations about Plugs: ThePlug[]				***/
/********************						***********************/

enum PPPlugModes {
	MADPlugImport =				(OSType)'IMPL',
	MADPlugExport =				(OSType)'EXPL',
	MADPlugInfo =				(OSType)'INFO',
	MADPlugTest =				(OSType)'TEST',
	MADPlugPlay =				(OSType)'PLAY',
	MADPlugImportExport =		(OSType)'EXIM',
	MADPlugSampleImporter =		(OSType)'SAMP',
	MADPlugInstrumentImporter =	(OSType)'INST',
	MADPlugNonePlug =			(OSType)'NONE'
};

#pragma pack(pop)

#ifdef _MAC_H

#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFURL.h>
#if !TARGET_OS_IPHONE
#include <CoreFoundation/CFBundle.h>
#endif

typedef OSErr (*MADPLUGFUNC)(OSType, char *, MADMusic *, PPInfoRec *, MADDriverSettings *);

typedef struct PlugInfo
{
	OSType		mode;			// Mode support : Import +/ Export
	UInt32		version;		// Plug-in version
	MADPLUGFUNC	IOPlug;			// Plug CODE
	CFStringRef	MenuName;		// Plug name
	CFStringRef	AuthorString;	// Plug author
#if !TARGET_OS_IPHONE
	CFBundleRef	file;			// Location of plug file
#endif
	CFArrayRef	UTItypes;		// CFStrings of supported UTIs
	char		type[5];		// OSType of file support.
} PlugInfo;
#endif

#ifdef WIN32
#include <windows.h>
typedef OSErr (*PLUGDLLFUNC)(OSType, char *, MADMusic *, PPInfoRec *, MADDriverSettings *);
typedef struct PlugInfo
{
	OSType		mode;				// Mode support : Import +/ Export
	int			version;			// Plug-in version
	HMODULE		hLibrary;
	PLUGDLLFUNC	IOPlug;				// Plug CODE
	char		MenuName[65];		// Plug name
	char		AuthorString[65];	// Plug author
	char		file[MAX_PATH * 2];	// Location of plug file
	char		type[5];			// OSType of file support
} PlugInfo;
#endif

#ifdef _BE_H
//TODO: include BeOS headers!
typedef	OSErr (*MADPlug)(OSType order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

typedef struct PlugInfo
{
	OSType		mode;				// Mode support : Import +/ Export
	int			version;			// Plug-in version
	image_id	hLibrary;
	MADPlug		IOPlug;				// Plug CODE
	char		MenuName[65];		// Plug name
	char		AuthorString[65];	// Plug author
	char		file[1024];			// Location of plug file
	char		type[5];			// OSType of file support
} PlugInfo;
#endif

#if (defined(__UNIX__) && !(defined (_MAC_H) || defined (_BE_H)))
#include <dlfcn.h>
#include <sys/param.h>  //For PATH_MAX
typedef OSErr (*MADPLUGFUNC)(OSType, char *, MADMusic *, PPInfoRec *, MADDriverSettings *);
typedef struct PlugInfo
{
	OSType		mode;				// Mode support : Import +/ Export
	int			version;			// Plug-in version
	void*		hLibrary;
	MADPLUGFUNC	IOPlug;				// Plug CODE
	char		MenuName[65];		// Plug name
	char		AuthorString[65];	// Plug author
	char		file[PATH_MAX];		// Location of plug file
	char		type[5];			// OSType of file support
} PlugInfo;
#endif

/********************						***********************/
/*** 		Global structure : PlayerPRO variables				***/
/********************						***********************/

typedef struct MADLibrary
{
	/** Plugs Import/Export variables **/
	SInt32		mytab[12];
	PlugInfo	*ThePlug;	// Pointers on plugs code & infos
	
	OSType		IDType;		// IDType = 'MADD' -- READ ONLY --
	short		TotalPlug;	// Number of Plugs in pointer ThePlug
} MADLibrary;

#ifndef __callback
#define __callback
#endif

typedef struct AEffect AEffect;

struct AEffect
{
	SInt32 magic;
	SInt32 (__callback *dispatcher)(AEffect *effect, SInt32 opCode, SInt32 index, SInt32 value, void *ptr, float opt);
	void (__callback *process)(AEffect *effect, float **inputs, float **outputs, SInt32 sampleframes);
	void (__callback *setParameter)(AEffect *effect, SInt32 index, float parameter);
	float (__callback *getParameter)(AEffect *effect, SInt32 index);
	
	SInt32 numPrograms;
	SInt32 numParams;
	SInt32 numInputs;
	SInt32 numOutputs;
	SInt32 flags;
	SInt32 resvd1;
	SInt32 resvd2;
	SInt32 initialDelay;
	SInt32 realQualities;
	SInt32 offQualities;
	float ioRatio;
	void *object;
	void *user;
	SInt32 uniqueID;
	SInt32 version;
	void (__callback *processReplacing)(AEffect *effect, float **inputs, float **outputs, SInt32 sampleframes);
	char future[60];
};

typedef	SInt32 (*audioMasterCallback)(AEffect *effect, SInt32 opcode, SInt32 index, SInt32 value, void *ptr, float opt);
	
typedef AEffect *(*VSTPlugInPtr)(audioMasterCallback  cb);

#if defined(_MAC_H) && !TARGET_OS_IPHONE
//TODO: update VST headers
typedef struct __VSTEffect
{
	AEffect				*ce[2];
	short				VSTid;
	CFStringRef			name;
	Boolean				Active;
	CFBundleRef			connID;
	VSTPlugInPtr		vstMain;
	Boolean				ProcessReplacingNotAvailable;
} VSTEffect;
#endif

#ifdef WIN32
typedef struct __VSTEffect
{
	AEffect				*ce[2];
	short				VSTid;
	char				name[50];
	Boolean				Active;
	HMODULE				connID;
	VSTPlugInPtr		vstMain;
	Boolean				ProcessReplacingNotAvailable;
} VSTEffect;
#endif

#if (defined(__UNIX__) && !(defined (_MAC_H) || defined (_BE_H)))
typedef struct __VSTEffect
{
	AEffect				*ce[2];
	short				VSTid;
	char				name[50];
	Boolean				Active;
	void				*connID;
	VSTPlugInPtr		vstMain;
	Boolean				ProcessReplacingNotAvailable;
} VSTEffect;
#endif

typedef struct __MADDriverRec MADDriverRec;

/********************						***********************/
/*** 					   EFFECTS ID							***/
/********************						***********************/

enum MADEffectsID {
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

/**
 @function	PPDebugStr
 @abstract	PlayerPROCore's internal debugger function
 @param		line
 The line number
 @param		file
 The file that has the problem
 @param		text
 Developer text that is used to help debug the issue. IT CANNOT BE NULL, although a blank string is fine.
 @discussion NORMALLY it is never called, only when a FATAL error has occured.
 This function is usually invoked using the macros __LINE__ and __FILE__ for the line and file paramaters.
 */
PPEXPORT void	PPDebugStr(short line, const char* file, const char* text);
	
/**
 @function PPRegisterDebugFunc
 @abstract used to set a delegate for PPDebugStr
 @param debugFunc
 The function to call when PPDebugStr is called, hopefully to have your app fail gracefully instead of instantly calling abort()
 @discussion Use this function to call your own debug function when PPDebugStr is called, otherwise calls to PPDebugStr will crash your app.
 You can reset to the default PPDebugStr implementation by calling this function with NULL.
 */
PPEXPORT void	PPRegisterDebugFunc(void (__callback *debugFunc)(short, const char*, const char*));

/**
 @function MADInitLibrary
 @abstract MADLibrary initialization
 @result The error encountered, if any. Will be of type MADErrors unless zero (noErr)
 @param PlugsFolderName
 The folder location for the plug-ins for PlayerPROCore to look for.
 On certain platforms, this can be NULL
 @param MADLib
 Usually a pointer passed by reference. If successful, it will return an initialized MADLibrary struct.
 @discussion You must call this function if you want to use other functions & variables.
 Needed to help set up the MADDriver structure and load non-MADK audio trackers.
 */
PPEXPORT OSErr	MADInitLibrary(const char *PlugsFolderName, MADLibrary **MADLib);

PPEXPORT OSErr	MADDisposeLibrary(MADLibrary *MADLib);						// Close Library, close music, close driver, free all memory

PPEXPORT void	MADGetBestDriver(MADDriverSettings *DriverInitParam);		// Found and identify the current Mac sound hardware and fill DriverInitParam
PPEXPORT Boolean MADSoundDriverIsAvalable(short theDriver);
PPEXPORT int	MADSoundDriverList();

PPEXPORT OSErr	MADCreateDriver(MADDriverSettings *DriverInitParam, MADLibrary *MADLib, MADDriverRec** returnDriver);		// Music Driver initialization and memory allocation
PPEXPORT OSErr	MADDisposeDriver(MADDriverRec *MDriver);											// Dispose the music driver, use it after RInitMusic()

PPEXPORT OSErr	MADChangeDriverSettings(MADDriverSettings *DriverInitParam, MADDriverRec** returnDriver);

PPEXPORT MADDriverSettings MADGetDriverSettings(MADDriverRec *theDriver);

PPEXPORT OSErr	MADStartDriver(MADDriverRec *MDriver);										// NEW - Activates the sound generating procedure (interruption)
PPEXPORT OSErr	MADStopDriver(MADDriverRec *MDriver);										// NEW - DEActivates the sound generating procedure (interruption)

PPEXPORT OSErr	MADPlayMusic(MADDriverRec *MDriver);										// NEW - Read and play current music in memory - Call MADStartInterruption BEFORE
PPEXPORT OSErr	MADStopMusic(MADDriverRec *MDriver);										// NEW - Stop reading current music in memory, Use MADCleanDriver to stop sounds
PPEXPORT Boolean MADIsPlayingMusic(MADDriverRec *driver);									// NEW - See if PlayerPRO is playing music

PPEXPORT void	MADCleanDriver(MADDriverRec *intDriver);									// Clean the driver : stop playing sounds

PPEXPORT OSErr	MADReset(MADDriverRec *MDriver);											// Reset the current music at the start position
PPEXPORT OSErr	MADGetMusicStatus(MADDriverRec *MDriver, long *fullTime, long *curTime);			// Get informations about music position and duration, IN 1/60th SECS !! NOT IN SECS ANYMORE !!!!!!!
PPEXPORT OSErr	MADSetMusicStatus(MADDriverRec *MDriver, long minV, long maxV, long curV);			// Change position of current music, by example MADSetMusicStatus( 0, 100, 50) = go to the middle of the music

PPEXPORT OSErr	MADAttachDriverToMusic(MADDriverRec *driver, MADMusic *music, char*);

PPEXPORT OSErr	MADLoadMusicPtr(MADMusic **music, char *myPtr);								// MAD ONLY - Load a MAD Ptr into memory, you can free() your Ptr after this call

PPEXPORT OSErr	MADLoadMusicFileCString(MADLibrary *, MADMusic **music, char *type, char *fName);			// Load a music file with plugs

PPEXPORT OSErr	MADMusicIdentifyCString(MADLibrary *, char *type, char *cName);			// Identify what kind of music format is cName file.

PPEXPORT OSErr	MADMusicInfoCString(MADLibrary *lib, char *type, char* cName, PPInfoRec *InfoRec);
	
PPEXPORT OSErr	MADMusicExportCString(MADLibrary *lib, MADMusic *music, char *type, char* cName);

PPEXPORT OSErr	MADMusicSaveCString(MADMusic *music, const char *cName, Boolean compressMAD);
//PPEXPORT OSErr	MADMusicSavePointer(MADMusic *music, void **outPtr, size_t *outPtrSize, Boolean compressMAD);

#ifdef _MAC_H
PPEXPORT OSErr	MADLoadMusicCFURLFile(MADLibrary *lib, MADMusic **music, char *type, CFURLRef theRef);
PPEXPORT OSErr	MADMusicIdentifyCFURL(MADLibrary *lib, char *type, CFURLRef URLRef);
PPEXPORT OSErr	MADMusicInfoCFURL(MADLibrary *lib, char *type, CFURLRef theRef, PPInfoRec *InfoRec);
PPEXPORT OSErr	MADMusicExportCFURL(MADLibrary *lib, MADMusic *music, char *type, CFURLRef fileURL);
PPEXPORT OSErr	MADMusicSaveCFURL(MADMusic *music, CFURLRef urlRef, Boolean compressMAD);
#endif

PPEXPORT Boolean MADPlugAvailable(MADLibrary *, char *type);								// Is plug 'type' available?

PPEXPORT OSErr	MADDisposeMusic(MADMusic **, MADDriverRec *MDriver);								// Dispose the current music, use it after RLoadMusic(), RLoadMusicRsrc(), RInstallMADF()

PPEXPORT void	MADChangeTracks(MADDriverRec *MDriver, short);				// Change current tracks number of the music driver
PPEXPORT Cmd*	GetMADCommand(short		position,						// Extract a Command from a PatData structure
							  short		channel,
							  PatData*	aPatData);

PPEXPORT OSErr	MADPlaySoundData(MADDriverRec	*MDriver,
								 const char		*soundPtr,				// Sound Pointer to data
								 size_t			size,					// Sound size in bytes
								 int			channel,				// channel ID on which to play sound
								 Byte			note,					// note: 0 to NUMBER_NOTES or 0xFF: play sound at 22 Khz
								 int			amplitude,				// 8 or 16 bits
								 size_t			loopBeg,				// loop beginning
								 size_t			loopSize,				// loop size in bytes
								 unsigned int	rate,					// sample rate of the sound data, by ex: rate22khz
								 Boolean		stereo);				// sample is in stereo or in mono?
							
#if 0
PPEXPORT OSErr	MADPlaySoundDataSYNC(MADDriverRec	*MDriver,
									 char			*soundPtr,				// Sound Pointer to data
									 long			size,					// Sound size in bytes
									 long			channel,				// channel ID on which to play sound
									 long			note,					// note: 0 to NUMBER_NOTES or 0xFF: play sound at 22 Khz
									 long			amplitude,				// 8 or 16 bits
									 long			loopBeg,				// loop beginning
									 long			loopSize,				// loop size in bytes
									 double			rate,					// sample rate of the sound data, by ex: rate22khz
									 Boolean		stereo);				// sample is in stereo or in mono?
#endif

PPEXPORT Boolean MADIsDonePlaying(MADDriverRec *MDriver);
	
PPEXPORT void	MADBeginExport(MADDriverRec *driver);
PPEXPORT void	MADEndExport(MADDriverRec *driver);
PPEXPORT Boolean MADIsExporting(MADDriverRec *driver);

PPEXPORT Boolean MADWasReading(MADDriverRec *driver) DEPRECATED_ATTRIBUTE;
PPEXPORT void	MADSetReading(MADDriverRec *driver, Boolean toSet) DEPRECATED_ATTRIBUTE;

PPEXPORT SInt32	MADAudioLength(MADDriverRec *theRec);

PPEXPORT size_t MADGetMusicSize(MADMusic*);

PPEXPORT void	MADDriverClearChannel(MADDriverRec *theRec, int channel);

PPEXPORT Boolean MADDriverChannelIsDonePlaying(MADDriverRec *theRec, int chan);

#ifdef __cplusplus
}
#endif

#endif
