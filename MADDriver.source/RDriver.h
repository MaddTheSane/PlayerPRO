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

/*!
 *	@header	RDriver.h
 *	@abstract The data types used internally by PlayerPROCore.
 */

#ifndef __RDRIVERH__
#define __RDRIVERH__

#include "MAD.h"
#include <limits.h>
#ifdef _MAC_H
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFBundle.h>
#endif

#ifdef _BE_H
#include <OS.h>
#include <Entry.h>
#include <File.h>
#include <NodeInfo.h>
#endif

#if (defined(__ELF__) && !(defined (_MAC_H) || defined (_BE_H)))
#include <dlfcn.h>
#include <sys/param.h>  //For PATH_MAX
#endif
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
/*** 					   EFFECTS ID							***/
/********************						***********************/

typedef MADENUM(MADByte, MADEffectID) {
	MADEffectArpeggio		= 0,	//	0x00
	MADEffectDownslide		= 1,	//	0x01
	MADEffectUpslide 		= 2,	//	0x02
	MADEffectPortamento 	= 3,	//	0x03
	MADEffectVibrato 		= 4,	//	0x04
	MADEffectPortaSlide 	= 5,	//	0x05
	MADEffectVibratoSlide	= 6,	//	0x06
	MADEffectNothing 		= 7,	//	0x07
	MADEffectPanning		= 8,	//	0x08
	MADEffectOffset 		= 9,	//	0x09
	MADEffectSlideVolume 	= 10,	//	0x0A
	MADEffectFastSkip		= 11,	//	0x0B
	MADEffectVolume 		= 12,	//	0x0C
	MADEffectSkip			= 13,	//	0x0D
	MADEffectExtended		= 14,	//	0x0E
	MADEffectSpeed			= 15,	//	0x0F
	MADEffectNoteOff 		= 16,	//	0x0G
	MADEffectLoop			= 17,	// 	0x0L
	MADEffectNOffset		= 18	//  0x0O
};

/********************						***********************/
/*** 			   MADChannel structure definition					***/
/********************						***********************/

typedef struct MADChannel
{
	int		ID;					// Channel ID - 0 to MAXTRACK
	int		TrackID;			// TrackID - 0 to MAXTRACK (Used in multiChannel mode)
	
	char	*begPtr;			// Sample Data char* - Beginning of data
	char	*maxPtr;			// Sample Data char* - End of data
	char	*curPtr;			// Sample Data char* - Current position
	size_t	sizePtr;			// Sample Size in bytes
	
	int		amp;				// Sample amplitude: 8 or 16 bits
	
	uintptr_t	loopBeg;		// Loop Beginning
	size_t		loopSize;		// Loop Size
	
	int		ins;				// Current Instrument ID
	int		insOld;				// Previous Instrument ID played on this MADChannel
	int		samp;				// Current Sample ID
	
	int		fineTune;			// Finetune
	
	int		note;				// Note
	int		noteOld;			// Previous note played on this MADChannel
	int		relNoteOld;			// Previous realnote
	
	int		period;				// Current period
	int		periodOld;			// Previous period played on this MADChannel
	
	int		vol;				// Channel vol (0 to 64)
	int		pann;				// Channel pan (0 to 64)
	
	MADEffectID	cmd;			// Command
	MADByte		arg;			// Argument of command
	MADByte		volcmd;			// Volume Command
	
	int		arp[MAX_ARP];		// Used for arpeggio command
	int		arpindex;			// Used for arpeggio command
	bool	arpUse;
	
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
	
	bool	KeyOn;
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
	
	bool	volEnvActive, pannEnvActive;
	
	short	aaa;
	short	bbb;
	float	ppp;
	int		panPitch;
	
	int		lAC;
	
	char	*prevPtr;
	int		lastWordL, curLastWordL;
	int		lastWordR, curLastWordR;
	int		curLevelL, curLevelR;
	
	bool	LevelDirectionL, LevelDirectionR, RemoverWorking;
	
	int		prevVol0;
	int		prevVol1;
	
	/**/
	
	bool	GEffect;
	short	GPat, GReader;
	
	/**/
	
	bool	stereo;
	
	MADLoopType	loopType;
	bool		pingpong;
	
	int		preOff;
	char	preVal, preVal2;
	char	preValR, preVal2R;
	
	short	spreVal, spreVal2;
	short	spreValR, spreVal2R;
	int		TICKREMOVESIZE;
	
	
	bool	PanningE8;
	short	trig;
	short	PatternLoopE6, PatternLoopE6Count, PatternLoopE6ID;
	
	int		TimeCounter;
} MADChannel;

/********************						***********************/
/*** 		Music description - used in Import/Export filter	***/
/********************						***********************/

typedef struct MADMusic {
	int			position, fullTime;
	MADSpec		*header;					// Music Header - See 'MAD.h'
	PatData		*partition[MAXPATTERN];		// Patterns
	InstrData	*fid;						// Instruments
	sData		**sample;					// Samples
	FXSets		*sets;						// FXSettings
	MADFourChar	originalFormat;
	MADPStr255	musicFileName;				// Pascal string
	bool		musicUnderModification;		// Tell the driver to NOT access music data
	bool		hasChanged;
} MADMusic;

/********************						***********************/
/*** 			     Driver Settings definition					***/
/********************						***********************/

/*!
 *	@enum		MADSoundOutput
 *	@abstract	Driver Settings definition
 *	@seealso	MADSoundOutputBit
 *	@seealso	MADSoundDriverList
 *	@constant	CoreAudioDriver
 *				OSX ONLY Core Audio driver
 *	@constant	DirectSound95NT
 *				Only available on Windows
 *	@constant	Wave95NT
 *				Only available on Windows
 *	@constant	PortAudioDriver
 *				Can be present on multiple OSes
 *	@constant	PulseAudioDriver
 *				Mostly used by Linux/Unix
 *	@constant	ESDDriver
 *				ESound Driver. available on most UNIX Systems
 *	@constant	BeOSSoundDriver
 *				BeOS/Haiku Only
 *	@constant	MIDISoundDriver
 *				Not yet available
 *	@constant	ASIOSoundManager
 *				ASIO Sound Driver by Steinberg<br>NOT Available
 *	@constant	SoundManagerDriver
 *				Deprecated and removed
 *	@constant	NoHardwareDriver
 *				NO HARDWARE CONNECTION, will not produce any sound
 */
typedef MADENUM(short, MADSoundOutput) {
	CoreAudioDriver = 1,
	DirectSound95NT,
	Wave95NT,
	PortAudioDriver,
	PulseAudioDriver,
	ESDDriver,
	BeOSSoundDriver,
	MIDISoundDriver,
	ASIOSoundManager,
	SoundManagerDriver,
	NoHardwareDriver = SHRT_MAX
};

/*!
 *	@enum		MADSoundOutputBit
 *	@abstract	Bit values of \c MADSoundOutput
 *	@seealso	MADSoundOutput
 *	@seealso	MADSoundDriverList
 */
typedef MADOPTIONS(unsigned int, MADSoundOutputBit) {
	MIDISoundDriverBit	= 1 << MIDISoundDriver,
	BeOSSoundDriverBit	= 1 << BeOSSoundDriver,
	DirectSound95NTBit	= 1 << DirectSound95NT,
	Wave95NTBit			= 1 << Wave95NT,
	CoreAudioDriverBit	= 1 << CoreAudioDriver,
	ESDDriverBit		= 1 << ESDDriver,
	PortAudioDriverBit	= 1 << PortAudioDriver,
	PulseAudioDriverBit	= 1 << PulseAudioDriver,
	ASIOSoundManagerBit	= 1 << ASIOSoundManager,
	SoundManagerDriverBit= 1 << SoundManagerDriver
};

/*!
 *	@enum		MADOutputChannel
 *	@abstract	The output channels to output to
 *	@constant	MonoOutPut
 *				NOT SUPPORTED anymore
 *	@constant	StereoOutPut
 *				NOT SUPPORTED anymore
 *	@constant	DeluxeStereoOutPut
 *				USE THIS ONE ONLY !!!!!!!!!!!!!!
 *	@constant	PolyPhonic
 *				Do NOT use this! Standard hardware doesn't support it!
 */
typedef MADENUM(short, MADOutputChannel) {
	MonoOutPut = 1,
	StereoOutPut,
	DeluxeStereoOutPut,
	PolyPhonic
};

#define oldMonoOutPut MonoOutPut
#define oldStereoOutPut StereoOutPut

#pragma pack(push, 8)
/*!
 *	@struct	MADDriverSettings
 *	@var	numChn
 *			Active tracks from 2 to 32, automatically setup when a new music is loaded
 *	@var	outPutBits
 *			8 or 16 Bits TODO: 24 Bits
 *	@var	outPutMode
 *			Now, only \c DeluxeStereoOutPut is available!
 *	@var	driverMode
 *			The Playback driver
 *	@var	outPutRate
 *			Audio Sample Rate
 *	@var	MicroDelaySize
 *			Micro delay duration (in ms, max 1 sec = 1000 ms, min = 0 ms
 *	@var	ReverbSize
 *			Reverb delay duration (in ms, min = 25 ms, max 1 sec = 1000 ms)
 *	@var	ReverbStrength
 *			Reverb strength in % (0 <-> 70)
 *	@var	oversampling
 *			OverSampling value, 1 = normal; works on select compilers
 *	@var	TickRemover
 *			Remove volume/sample/loop ticks
 *	@var	surround
 *			Surround effect active?
 *	@var	Reverb
 *			Reverb effect active?
 *	@var	repeatMusic
 *			If music finished, repeat it or stop.
 *	@var	reserved
 *			For future use.
 */
typedef struct MADDriverSettings {
	short				numChn;
	short				outPutBits;
	MADOutputChannel	outPutMode;
	MADSoundOutput		driverMode;
	unsigned int		outPutRate;
	int		MicroDelaySize;
	int		ReverbSize;
	int		ReverbStrength;
	int		oversampling;
	bool	TickRemover;
	bool	surround;
	bool	Reverb;
	bool	repeatMusic;
	union reserved {
		char padding[44];
	} reserved;
} MADDriverSettings;
#pragma pack(pop)

/******************************************************************/
//******************* MUSICS IMPORT/EXPORT PLUGS  *****************/
//
//	Protocol Version 2.3
//
//	To use with PlayerPRO for CodeWarrior
//
//	Your main function have to be in this form:
//	MADErr PPImpExpMain(MADFourChar order,
//						char *AlienFileName,
//						MADMusic *MadFile,
//						MADInfoRec *info,
//						MADDriverSettings *DriverParam);
//
//	Actual plug have to support these orders:
//
//	order: 'TEST':	check the AlienFile to see if your Plug really supports it.
//	order: 'IMPL':	convert the AlienFile into a MADMusic. You have to allocate MADMusic.
//	order: 'INFO':	Fill MADInfoRec structure.
//	order: 'EXPL':	Convert the MADMusic into AlienFile. You have to create the AlienFile.
//					Don't delete the MADMusic Structure after conversion !!
//
//	An IMPORT plug have to support these orders: 'TEST', 'IMPL', 'INFO'
//	An EXPORT plug have to support these orders: 'EXPL'
// 	An IMPORT/EXPORT plug have to support these orders: 'TEST', 'IMPL', 'INFO', 'EXPL'
//
//
/********************						***********************/

typedef struct MADInfoRec {
	int		totalPatterns;
	int		partitionLength;
	
	long	fileSize;
	
	MADFourChar	signature;
	
	short	totalTracks;
	short	totalInstruments;
	
	char	internalFileName[60];
	char	formatDescription[60];
} MADInfoRec;

/********************						***********************/
/*** 			Informations about Plugs: ThePlug[]				***/
/********************						***********************/

typedef MADENUM(MADFourChar, MADPlugModes) {
	MADPlugImport =				'IMPL',
	MADPlugExport =				'EXPL',
	MADPlugInfo =				'INFO',
	MADPlugTest =				'TEST',
	MADPlugPlay =				'PLAY',
	MADPlugImportExport =		'EXIM',
	MADPlugSampleImporter =		'SAMP',
	MADPlugInstrumentImporter =	'INST',
	MADPlugNonePlug =			'NONE'
};

#pragma pack(pop)

#ifdef _MAC_H

typedef MADErr (*MADPLUGFUNC)(MADFourChar, char *, MADMusic *, MADInfoRec *, MADDriverSettings *);

typedef struct PlugInfo
{
	OSType		mode;			// Mode support : Import +/ Export
	UInt32		version;		// Plug-in version
	MADPLUGFUNC	IOPlug;			// Plug CODE
	CFStringRef	MenuName;		// Plug name
	CFStringRef	AuthorString;	// Plug author
	CFBundleRef	file;			// Location of plug file
	CFArrayRef	UTItypes;		// CFStrings of supported UTIs
	char		type[5];		// OSType of file support.
} PlugInfo;
#endif

#ifdef WIN32
#include <windows.h>
typedef MADErr (*PLUGDLLFUNC)(MADFourChar, char *, MADMusic *, MADInfoRec *, MADDriverSettings *);

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
typedef	MADErr (*MADPlug)(MADFourChar order, char *AlienFileName, MADMusic *MadFile, MADInfoRec *info, MADDriverSettings *init);

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
typedef MADErr (*MADPLUGFUNC)(MADFourChar, char *, MADMusic *, MADInfoRec *, MADDriverSettings *);
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

typedef struct MADLibrary {
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
	MADFourChar magic;
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
	MADFourChar uniqueID;
	int version;
	void (__callback *processReplacing)(AEffect *effect, float **inputs, float **outputs, int sampleframes);
	char future[60];
};

typedef	int (*audioMasterCallback)(AEffect *effect, int opcode, int index, int value, void *ptr, float opt);
	
typedef AEffect *(*VSTPlugInPtr)(audioMasterCallback cb);

#if defined(_MAC_H)

//TODO: update VST headers
typedef struct VSTEffect {
	AEffect			*ce[2];
	short			VSTid;
	CFStringRef		name;
	bool			Active;
	CFBundleRef		connID;
	VSTPlugInPtr	vstMain;
	bool			ProcessReplacingNotAvailable;
} VSTEffect;

#elif defined(WIN32)

typedef struct VSTEffect {
	AEffect			*ce[2];
	short			VSTid;
	char			name[50];
	bool			Active;
	HMODULE			connID;
	VSTPlugInPtr	vstMain;
	bool			ProcessReplacingNotAvailable;
} VSTEffect;

#elif (defined(__ELF__) && !(defined (_MAC_H) || defined (_BE_H)))

typedef struct VSTEffect {
	AEffect			*ce[2];
	short			VSTid;
	char			name[50];
	bool			Active;
	void			*connID;
	VSTPlugInPtr	vstMain;
	bool			ProcessReplacingNotAvailable;
} VSTEffect;
#endif

typedef struct MADDriverBase {
	/**********************/
	/** Public variables **/
	/**********************/
	
	/**  Current music in memory, loaded with RLoadMusic() by example **/
	
	MADMusic		*curMusic;										// Current music played by this driver, it can be NULL !!!
	MADLibrary		*lib;
	
	/**  Drivers variables **/
	
	MADChannel			chan[MAXTRACK];									// Current driver channels -- READ ONLY --
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

} MADDriverBase;

typedef struct MADDriverRec MADDriverRec, *MADDriverRecPtr;

/********************						***********************/
/*** 					   FUNCTIONS							***/
/********************						***********************/


#ifdef __cplusplus
extern "C" {
#endif

#pragma mark Debug functions that will crash

/*!
 *	@function	MADDebugStr
 *	@abstract	PlayerPROCore's internal debugger function
 *	@param		line
 *				The line number
 *	@param		file
 *				The file that has the problem
 *	@param		text
 *				Developer text that is used to help debug the issue. IT CANNOT BE NULL,
 *				although an empty string is fine.
 *	@discussion NORMALLY it is never called, only when a FATAL error has occured. <br>
 *				This function is usually invoked using the macros \c __LINE__ and
 *				\c __FILE__ for the line and file paramaters.
 */
PPEXPORT void	MADDebugStr(short line, const char* file, const char* text);
	
/*!
 *	@function	MADRegisterDebugFunc
 *	@abstract	used to set a callback for MADDebugStr
 *	@param		debugFunc
 *				The function to call when MADDebugStr is called, hopefully to have your
 *				app fail gracefully instead of instantly calling \c abort()
 *	@discussion	Use this function to call your own debug function when MADDebugStr is
 *				called, otherwise calls to MADDebugStr will crash your app.<br>
 *				You can reset to the default MADDebugStr implementation by calling this
 *				function and passing \c NULL to it.
 */
PPEXPORT void	MADRegisterDebugFunc(void (__callback *debugFunc)(short, const char*, const char*));

#ifdef __BLOCKS__
/*!
 *	@function	MADRegisterDebugBlock
 *	@abstract	used to set a callback for MADDebugStr
 *	@param		newdebugBlock
 *				The block to call when \c MADDebugStr is called, hopefully to have your app
 *				fail gracefully instead of instantly calling \c abort()
 *	@discussion	Use this function to call your own debug function when MADDebugStr is
 *				called, otherwise calls to MADDebugStr will crash your app.
 *				You can reset to the default MADDebugStr implementation by calling this
 *				function and passing \c NULL to it. <br>
 *				This function is only available if your compiler supports blocks (Clang),
 *				otherwise it is unavailable. <br>
 *				If PlayerPROCore was built without blocks support and you try to call this
 *				function, the linker won't be able to find the function.
 */
PPEXPORT void MADRegisterDebugBlock(void (^newdebugBlock)(short, const char*, const char*));
#endif

#pragma mark library initialization
	
/*!
 *	@function	MADInitLibrary
 *	@abstract	MADLibrary initialization
 *	@result		The error encountered, if any. Will be of type MADErrors unless \c MADNoErr
 *	@param		PlugsFolderName
 *				The folder location for the plug-ins for PlayerPROCore to look for. <br>
 *				On certain platforms, this can be \c NULL
 *	@param		MADLib
 *				Usually a pointer passed by reference. If successful, it will return an initialized \c MADLibrary struct.
 *	@return		An error type on failure, or \c MADNoErr on success
 *	@discussion	You must call this function if you want to use other functions & variables.<br>
 *				Needed to help set up the MADDriver structure and load non-MADK audio trackers.
 */
PPEXPORT MADErr	MADInitLibrary(const char *PlugsFolderName, MADLibrary **MADLib);

/*!
 *	@function	MADDisposeLibrary
 *	@abstract	Close Library, free all memory
 *	@param		MADLib
 *				The library to destroy
 *	@return		An error type on failure, or \c MADNoErr on success
 */
PPEXPORT MADErr	MADDisposeLibrary(MADLibrary *MADLib);

/*!
 *	@function	MADPlugAvailable
 *	@abstract	Is plug \c 'type' available?
 *	@param		MADLib
 *				The library test for the availability of the plug-in
 *	@param		type
 *				The plug-in type to check for.
 *	@return		\c true if a the plug-in is available. Otherwise, \c false .
 */
PPEXPORT bool MADPlugAvailable(const MADLibrary *MADLib, const char *type);

#pragma mark back-end (driver) availability
/*!
 *	@function	MADGetBestDriver
 *	@abstract	Find and identify the current sound hardware and APIs and fill DriverInitParam.
 *	@param		DriverInitParam
 *				A pointer to the driver settings to modify. On return, is set to the best drivers
 *				for the current sound hardware and APIs.
 */
PPEXPORT void	MADGetBestDriver(MADDriverSettings *DriverInitParam);

/*!
 *	@function	MADSoundDriverIsAvalable
 *	@abstract	Identifies if a back-end is available or not.
 *	@param		theDriver
 *				The driver to test for availability.
 *	@return		A \c bool indicating if the driver is available or not.
 */
PPEXPORT bool	MADSoundDriverIsAvalable(MADSoundOutput theDriver);

/*!
 *	@function	MADSoundDriverList
 *	@abstract	Lists all the available sound back-ends available
 *	@return		A list of available sound drivers in a bit field.
 *	@seealso	MADSoundOutputBit
 */
PPEXPORT MADSoundOutputBit	MADSoundDriverList();

#pragma mark -
PPEXPORT MADErr	MADCreateDriver(MADDriverSettings *DriverInitParam, MADLibrary *MADLib, MADDriverRecPtr* returnDriver);		// Music Driver initialization and memory allocation
PPEXPORT MADErr	MADDisposeDriver(MADDriverRecPtr MDriver);											// Dispose the music driver, use it after RInitMusic()

PPEXPORT MADErr	MADChangeDriverSettings(MADDriverSettings *DriverInitParam, MADDriverRecPtr* returnDriver);

PPEXPORT MADDriverSettings MADGetDriverSettings(const MADDriverRecPtr theDriver);

PPEXPORT MADErr	MADStartDriver(MADDriverRecPtr MDriver);										// NEW - Activates the sound generating procedure (interruption)
PPEXPORT MADErr	MADStopDriver(MADDriverRecPtr MDriver);										// NEW - DEActivates the sound generating procedure (interruption)

PPEXPORT MADErr	MADPlayMusic(MADDriverRecPtr MDriver);										// NEW - Read and play current music in memory - Call MADStartInterruption BEFORE
PPEXPORT MADErr	MADStopMusic(MADDriverRecPtr MDriver);										// NEW - Stop reading current music in memory, Use MADCleanDriver to stop sounds
PPEXPORT bool MADIsPlayingMusic(MADDriverRecPtr driver);									// NEW - See if PlayerPRO is playing music

PPEXPORT void	MADCleanDriver(MADDriverRecPtr intDriver);									// Clean the driver : stop playing sounds

PPEXPORT MADErr	MADReset(MADDriverRecPtr MDriver);											// Reset the current music at the start position
PPEXPORT MADErr	MADGetMusicStatus(MADDriverRecPtr MDriver, long *fullTime, long *curTime);			// Get informations about music position and duration, IN 1/60th SECS !! NOT IN SECS ANYMORE !!!!!!!
PPEXPORT MADErr	MADSetMusicStatus(MADDriverRecPtr MDriver, long minV, long maxV, long curV);			// Change position of current music, by example MADSetMusicStatus(0, 100, 50) = go to the middle of the music

PPEXPORT MADErr	MADAttachDriverToMusic(MADDriverRecPtr driver, MADMusic *music, char* missingPlugins);

PPEXPORT MADErr	MADLoadMusicPtr(MADMusic **music, char *myPtr);								// MAD ONLY - Load a MAD char* into memory, you can free() your char* after this call

PPEXPORT MADErr	MADLoadMusicFileCString(MADLibrary *, MADMusic **music, char *type, char *fName);			// Load a music file with plugs

PPEXPORT MADErr	MADMusicIdentifyCString(MADLibrary *, char *type, char *cName);			// Identify what kind of music format is cName file.

PPEXPORT MADErr	MADMusicInfoCString(MADLibrary *lib, char *type, char* cName, MADInfoRec *InfoRec);
	
PPEXPORT MADErr	MADMusicExportCString(MADLibrary *lib, MADMusic *music, char *type, char* cName);

PPEXPORT MADErr	MADMusicSaveCString(MADMusic *music, const char *cName, bool compressMAD);
//PPEXPORT MADErr	MADMusicSavePointer(MADMusic *music, void **outPtr, size_t *outPtrSize, bool compressMAD);

#ifdef _MAC_H
#pragma mark OS X/iOS-only calls
PPEXPORT MADErr	MADLoadMusicCFURLFile(MADLibrary *lib, MADMusic **music, char *type, CFURLRef theRef);
PPEXPORT MADErr	MADMusicIdentifyCFURL(MADLibrary *lib, char *type, CFURLRef URLRef);
PPEXPORT MADErr	MADMusicInfoCFURL(MADLibrary *lib, char *type, CFURLRef theRef, MADInfoRec *InfoRec);
PPEXPORT MADErr	MADMusicExportCFURL(MADLibrary *lib, MADMusic *music, char *type, CFURLRef fileURL);
PPEXPORT MADErr	MADMusicSaveCFURL(MADMusic *music, CFURLRef urlRef, bool compressMAD);
#endif

PPEXPORT MADErr	MADDisposeMusic(MADMusic **, MADDriverRecPtr MDriver);								// Dispose the current music, use it after RLoadMusic(), RLoadMusicRsrc(), RInstallMADF()

PPEXPORT void	MADChangeTracks(MADDriverRecPtr MDriver, short);				// Change current tracks number of the music driver
PPEXPORT Cmd*	GetMADCommand(short		position,						// Extract a Command from a PatData structure
							  short		channel,
							  PatData*	aPatData);

PPEXPORT MADErr	MADPlaySoundData(MADDriverRecPtr	MDriver,
								 const char		*soundPtr,				// Sound Pointer to data
								 size_t			size,					// Sound size in bytes
								 int			channel,				// channel ID on which to play sound
								 MADByte		note,					// note: 0 to NUMBER_NOTES or 0xFF: play sound at 22 Khz
								 int			amplitude,				// 8 or 16 bits
								 size_t			loopBeg,				// loop beginning
								 size_t			loopSize,				// loop size in bytes
								 unsigned int	rate,					// sample rate of the sound data, by ex: rate22khz
								 bool			stereo);				// sample is in stereo or in mono?
							
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
									 bool			stereo);				// sample is in stereo or in mono?
#endif

#pragma mark MAD Driver functions
	
PPEXPORT bool	MADIsDonePlaying(MADDriverRecPtr MDriver);
	
PPEXPORT void	MADBeginExport(MADDriverRecPtr driver);
PPEXPORT void	MADEndExport(MADDriverRecPtr driver);
PPEXPORT bool	MADIsExporting(MADDriverRecPtr driver);

PPEXPORT bool	MADWasReading(MADDriverRecPtr driver) DEPRECATED_ATTRIBUTE;
PPEXPORT void	MADSetReading(MADDriverRecPtr driver, bool toSet) DEPRECATED_ATTRIBUTE;

PPEXPORT int	MADAudioLength(MADDriverRecPtr theRec);

PPEXPORT size_t MADGetMusicSize(const MADMusic *musSize);

PPEXPORT void	MADDriverClearChannel(MADDriverRecPtr theRec, int channel);

PPEXPORT bool	MADDriverChannelIsDonePlaying(MADDriverRecPtr theRec, int chan);

/*!
 *	@function	MADDriverGetBase
 *	@abstract	Get the base driver from a \c MADDriverRecPtr
 *	@param		theRec
 *				The driver to get the base info from
 *	@return		A pointer to a \c MADDriverBase struct.
 */
PPEXPORT MADDriverBase *MADDriverGetBase(MADDriverRecPtr theRec);

#pragma mark General Functions

PPEXPORT MADErr		MADKillInstrument(MADMusic*, short ins);
PPEXPORT MADErr		MADKillSample(MADMusic *, short ins, short sample);
PPEXPORT sData		*MADCreateSample(MADMusic *MDriver, short ins, short sample);
PPEXPORT MADErr		MADKillCmd(Cmd*);
PPEXPORT void		UpdateTracksNumber(MADDriverRecPtr);
PPEXPORT MADErr		MADCreateVolumeTable(MADDriverRecPtr intDriver);
PPEXPORT void		MADDisposeVolumeTable(MADDriverRecPtr intDriver);
PPEXPORT MADMusic*	CreateFreeMADK();
PPEXPORT short		FindAFreeChannel(MADDriverRecPtr intDriver);

#pragma mark Swift helpers
PPEXPORT IntPatData MADPatDataToInt(PatData *inPat);
	
#ifdef __cplusplus
}
#endif

#endif
