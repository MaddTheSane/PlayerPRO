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
	MADNoErr						= 0,
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
	
	char	*begPtr;			// Sample Data char* - Beginning of data
	char	*maxPtr;			// Sample Data char* - End of data
	char	*curPtr;			// Sample Data char* - Current position
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
    MADByte	arg;				// Argument of command
    MADByte	volcmd;				// Volume Command
	
	int		arp[MAX_ARP];		// Used for arpeggio command
	int		arpindex;			// Used for arpeggio command
    MADBool	arpUse;
	
	char	viboffset;			// Used for vibrato command
	int		vibdepth;			// Used for vibrato command
	int		vibrate;			// Used for vibrato command
	int		vibtype;			// Used for vibrato command
	
	int		slide;				// Used for slideUp and slideDown command
	
	int 	pitchgoal;			// Used for portamento command
	int 	pitchrate;			// Used for portamento command
	
	int 	volumerate;			// Used for slideVolume command
	
	int		oldArg[16];
	int		oldVibrate;
	int		oldVibdepth;
	
	int		eventTime;
	
	char	*samplePtr;			// Used internaly by MADPlaySoundData
	
	/**/
	
    MADBool	KeyOn;
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
	
    MADBool	volEnvActive, pannEnvActive;
	
	short	aaa;
	short	bbb;
	float	ppp;
	int		panPitch;
	
	int		lAC;
	
	char	*prevPtr;
	int		lastWordL, curLastWordL;
	int		lastWordR, curLastWordR;
	int		curLevelL, curLevelR;
	
    MADBool	LevelDirectionL, LevelDirectionR, RemoverWorking;
	
	int		prevVol0;
	int		prevVol1;
	
	/**/
	
    MADBool	GEffect;
	short	GPat, GReader;
	
	/**/
	
    MADBool	stereo;
	
    MADByte	loopType;
    MADBool	pingpong;
	
	int		preOff;
	char	preVal, preVal2;
	char	preValR, preVal2R;
	
	short	spreVal, spreVal2;
	short	spreValR, spreVal2R;
	int		TICKREMOVESIZE;
	
	
    MADBool	PanningE8;
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
    MADFourChar	originalFormat;
    MADPStr255	musicFileName;				// Pascal string
    MADBool		musicUnderModification;		// Tell the driver to NOT access music data
    MADBool		hasChanged;
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
    MADBool			TickRemover;		// Remove volume/sample/loop ticks.
    MADBool			surround;			// Surround effect active? true/false
    MADBool			Reverb;				// Reverb effect active? true/false
    MADBool			repeatMusic;		// If music finished, repeat it or stop.
} MADDriverSettings;

/******************************************************************/
//******************* MUSICS IMPORT/EXPORT PLUGS  *****************/
//
//	Protocol Version 2.3
//
//	To use with PlayerPRO for CodeWarrior
//
//	Your main function have to be in this form:
//	MADErr PPImpExpMain(	MADFourChar order,
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
	
    MADFourChar	signature;
	
	short	totalTracks;
	short	totalInstruments;
	
	char	internalFileName[60];
	char	formatDescription[60];
} PPInfoRec;


/********************						***********************/
/*** 			Informations about Plugs: ThePlug[]				***/
/********************						***********************/

enum PPPlugModes {
    MADPlugImport =				(MADFourChar)'IMPL',
    MADPlugExport =				(MADFourChar)'EXPL',
    MADPlugInfo =				(MADFourChar)'INFO',
    MADPlugTest =				(MADFourChar)'TEST',
    MADPlugPlay =				(MADFourChar)'PLAY',
    MADPlugImportExport =		(MADFourChar)'EXIM',
    MADPlugSampleImporter =		(MADFourChar)'SAMP',
    MADPlugInstrumentImporter =	(MADFourChar)'INST',
    MADPlugNonePlug =			(MADFourChar)'NONE'
};

#pragma pack(pop)

#ifdef _MAC_H

#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFURL.h>
#if !TARGET_OS_IPHONE
#include <CoreFoundation/CFBundle.h>
#endif

typedef MADErr (*MADPLUGFUNC)(MADFourChar, char *, MADMusic *, PPInfoRec *, MADDriverSettings *);

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
typedef MADErr (*PLUGDLLFUNC)(MADFourChar, char *, MADMusic *, PPInfoRec *, MADDriverSettings *);
typedef struct PlugInfo
{
    MADFourChar	mode;				// Mode support : Import +/ Export
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
typedef	MADErr (*MADPlug)(MADFourChar order, char *AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

typedef struct PlugInfo
{
    MADFourChar	mode;				// Mode support : Import +/ Export
	int			version;			// Plug-in version
	image_id	hLibrary;
	MADPlug		IOPlug;				// Plug CODE
	char		MenuName[65];		// Plug name
	char		AuthorString[65];	// Plug author
	char		file[1024];			// Location of plug file
	char		type[5];			// OSType of file support
} PlugInfo;
#endif

#if (defined(__ELF__) && !(defined (_MAC_H) || defined (_BE_H)))
#include <dlfcn.h>
#include <sys/param.h>  //For PATH_MAX
typedef MADErr (*MADPLUGFUNC)(MADFourChar, char *, MADMusic *, PPInfoRec *, MADDriverSettings *);
typedef struct PlugInfo
{
    MADFourChar	mode;				// Mode support : Import +/ Export
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
	int			mytab[12];
	PlugInfo	*ThePlug;	// Pointers on plugs code & infos
	
    MADFourChar	IDType;		// IDType = 'MADD' -- READ ONLY --
	short		TotalPlug;	// Number of Plugs in pointer ThePlug
} MADLibrary;

#ifndef __callback
#define __callback
#endif

typedef struct AEffect AEffect;

struct AEffect
{
	int magic;
	int (__callback *dispatcher)(AEffect *effect, int opCode, int index, int value, void *ptr, float opt);
	void (__callback *process)(AEffect *effect, float **inputs, float **outputs, int sampleframes);
	void (__callback *setParameter)(AEffect *effect, int index, float parameter);
	float (__callback *getParameter)(AEffect *effect, int index);
	
	int numPrograms;
	int numParams;
	int numInputs;
	int numOutputs;
	int flags;
	int resvd1;
	int resvd2;
	int initialDelay;
	int realQualities;
	int offQualities;
	float ioRatio;
	void *object;
	void *user;
	int uniqueID;
	int version;
	void (__callback *processReplacing)(AEffect *effect, float **inputs, float **outputs, int sampleframes);
	char future[60];
};

typedef	int (*audioMasterCallback)(AEffect *effect, int opcode, int index, int value, void *ptr, float opt);
	
typedef AEffect *(*VSTPlugInPtr)(audioMasterCallback cb);

#if defined(_MAC_H)

#if !TARGET_OS_IPHONE
//TODO: update VST headers
typedef struct __VSTEffect {
	AEffect			*ce[2];
	short			VSTid;
	CFStringRef		name;
	MADBool			Active;
	CFBundleRef		connID;
	VSTPlugInPtr	vstMain;
	MADBool			ProcessReplacingNotAvailable;
} VSTEffect;
#else
typedef struct __VSTEffect {
	
} VSTEffect;
#endif

#elif defined(WIN32)

typedef struct __VSTEffect {
	AEffect			*ce[2];
	short			VSTid;
	char			name[50];
	MADBool			Active;
	HMODULE			connID;
	VSTPlugInPtr	vstMain;
	MADBool			ProcessReplacingNotAvailable;
} VSTEffect;

#elif (defined(__ELF__) && !(defined (_MAC_H) || defined (_BE_H)))

typedef struct __VSTEffect {
	AEffect			*ce[2];
	short			VSTid;
	char			name[50];
    MADBool			Active;
	void			*connID;
	VSTPlugInPtr	vstMain;
    MADBool			ProcessReplacingNotAvailable;
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
 @result The error encountered, if any. Will be of type MADErrors unless zero (MADNoErr)
 @param PlugsFolderName
 The folder location for the plug-ins for PlayerPROCore to look for.
 On certain platforms, this can be NULL
 @param MADLib
 Usually a pointer passed by reference. If successful, it will return an initialized MADLibrary struct.
 @discussion You must call this function if you want to use other functions & variables.
 Needed to help set up the MADDriver structure and load non-MADK audio trackers.
 */
PPEXPORT MADErr	MADInitLibrary(const char *PlugsFolderName, MADLibrary **MADLib);

PPEXPORT MADErr	MADDisposeLibrary(MADLibrary *MADLib);						// Close Library, close music, close driver, free all memory

PPEXPORT void	MADGetBestDriver(MADDriverSettings *DriverInitParam);		// Found and identify the current Mac sound hardware and fill DriverInitParam
PPEXPORT MADBool MADSoundDriverIsAvalable(short theDriver);
PPEXPORT int	MADSoundDriverList();

PPEXPORT MADErr	MADCreateDriver(MADDriverSettings *DriverInitParam, MADLibrary *MADLib, MADDriverRec** returnDriver);		// Music Driver initialization and memory allocation
PPEXPORT MADErr	MADDisposeDriver(MADDriverRec *MDriver);											// Dispose the music driver, use it after RInitMusic()

PPEXPORT MADErr	MADChangeDriverSettings(MADDriverSettings *DriverInitParam, MADDriverRec** returnDriver);

PPEXPORT MADDriverSettings MADGetDriverSettings(MADDriverRec *theDriver);

PPEXPORT MADErr	MADStartDriver(MADDriverRec *MDriver);										// NEW - Activates the sound generating procedure (interruption)
PPEXPORT MADErr	MADStopDriver(MADDriverRec *MDriver);										// NEW - DEActivates the sound generating procedure (interruption)

PPEXPORT MADErr	MADPlayMusic(MADDriverRec *MDriver);										// NEW - Read and play current music in memory - Call MADStartInterruption BEFORE
PPEXPORT MADErr	MADStopMusic(MADDriverRec *MDriver);										// NEW - Stop reading current music in memory, Use MADCleanDriver to stop sounds
PPEXPORT MADBool MADIsPlayingMusic(MADDriverRec *driver);									// NEW - See if PlayerPRO is playing music

PPEXPORT void	MADCleanDriver(MADDriverRec *intDriver);									// Clean the driver : stop playing sounds

PPEXPORT MADErr	MADReset(MADDriverRec *MDriver);											// Reset the current music at the start position
PPEXPORT MADErr	MADGetMusicStatus(MADDriverRec *MDriver, long *fullTime, long *curTime);			// Get informations about music position and duration, IN 1/60th SECS !! NOT IN SECS ANYMORE !!!!!!!
PPEXPORT MADErr	MADSetMusicStatus(MADDriverRec *MDriver, long minV, long maxV, long curV);			// Change position of current music, by example MADSetMusicStatus(0, 100, 50) = go to the middle of the music

PPEXPORT MADErr	MADAttachDriverToMusic(MADDriverRec *driver, MADMusic *music, char*);

PPEXPORT MADErr	MADLoadMusicPtr(MADMusic **music, char *myPtr);								// MAD ONLY - Load a MAD char* into memory, you can free() your char* after this call

PPEXPORT MADErr	MADLoadMusicFileCString(MADLibrary *, MADMusic **music, char *type, char *fName);			// Load a music file with plugs

PPEXPORT MADErr	MADMusicIdentifyCString(MADLibrary *, char *type, char *cName);			// Identify what kind of music format is cName file.

PPEXPORT MADErr	MADMusicInfoCString(MADLibrary *lib, char *type, char* cName, PPInfoRec *InfoRec);
	
PPEXPORT MADErr	MADMusicExportCString(MADLibrary *lib, MADMusic *music, char *type, char* cName);

PPEXPORT MADErr	MADMusicSaveCString(MADMusic *music, const char *cName, MADBool compressMAD);
//PPEXPORT MADErr	MADMusicSavePointer(MADMusic *music, void **outPtr, size_t *outPtrSize, MADBool compressMAD);

#ifdef _MAC_H
PPEXPORT MADErr	MADLoadMusicCFURLFile(MADLibrary *lib, MADMusic **music, char *type, CFURLRef theRef);
PPEXPORT MADErr	MADMusicIdentifyCFURL(MADLibrary *lib, char *type, CFURLRef URLRef);
PPEXPORT MADErr	MADMusicInfoCFURL(MADLibrary *lib, char *type, CFURLRef theRef, PPInfoRec *InfoRec);
PPEXPORT MADErr	MADMusicExportCFURL(MADLibrary *lib, MADMusic *music, char *type, CFURLRef fileURL);
PPEXPORT MADErr	MADMusicSaveCFURL(MADMusic *music, CFURLRef urlRef, MADBool compressMAD);
#endif

PPEXPORT MADBool MADPlugAvailable(MADLibrary *, char *type);								// Is plug 'type' available?

PPEXPORT MADErr	MADDisposeMusic(MADMusic **, MADDriverRec *MDriver);								// Dispose the current music, use it after RLoadMusic(), RLoadMusicRsrc(), RInstallMADF()

PPEXPORT void	MADChangeTracks(MADDriverRec *MDriver, short);				// Change current tracks number of the music driver
PPEXPORT Cmd*	GetMADCommand(short		position,						// Extract a Command from a PatData structure
							  short		channel,
							  PatData*	aPatData);

PPEXPORT MADErr	MADPlaySoundData(MADDriverRec	*MDriver,
								 const char		*soundPtr,				// Sound Pointer to data
								 size_t			size,					// Sound size in bytes
								 int			channel,				// channel ID on which to play sound
                                 MADByte			note,					// note: 0 to NUMBER_NOTES or 0xFF: play sound at 22 Khz
								 int			amplitude,				// 8 or 16 bits
								 size_t			loopBeg,				// loop beginning
								 size_t			loopSize,				// loop size in bytes
								 unsigned int	rate,					// sample rate of the sound data, by ex: rate22khz
                                 MADBool		stereo);				// sample is in stereo or in mono?
							
#if 0
PPEXPORT MADErr	MADPlaySoundDataSYNC(MADDriverRec	*MDriver,
									 char			*soundPtr,				// Sound Pointer to data
									 long			size,					// Sound size in bytes
									 long			channel,				// channel ID on which to play sound
									 long			note,					// note: 0 to NUMBER_NOTES or 0xFF: play sound at 22 Khz
									 long			amplitude,				// 8 or 16 bits
									 long			loopBeg,				// loop beginning
									 long			loopSize,				// loop size in bytes
									 double			rate,					// sample rate of the sound data, by ex: rate22khz
									 MADBool		stereo);				// sample is in stereo or in mono?
#endif

PPEXPORT MADBool MADIsDonePlaying(MADDriverRec *MDriver);
	
PPEXPORT void	MADBeginExport(MADDriverRec *driver);
PPEXPORT void	MADEndExport(MADDriverRec *driver);
PPEXPORT MADBool MADIsExporting(MADDriverRec *driver);

PPEXPORT MADBool MADWasReading(MADDriverRec *driver) DEPRECATED_ATTRIBUTE;
PPEXPORT void	MADSetReading(MADDriverRec *driver, MADBool toSet) DEPRECATED_ATTRIBUTE;

PPEXPORT int	MADAudioLength(MADDriverRec *theRec);

PPEXPORT size_t MADGetMusicSize(MADMusic*);

PPEXPORT void	MADDriverClearChannel(MADDriverRec *theRec, int channel);

PPEXPORT MADBool MADDriverChannelIsDonePlaying(MADDriverRec *theRec, int chan);

#ifdef __cplusplus
}
#endif

#endif
