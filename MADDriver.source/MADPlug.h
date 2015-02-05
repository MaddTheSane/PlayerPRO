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

#ifndef __PPPLUGH__
#define __PPPLUGH__

#include "RDriver.h"
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>

typedef OSErr (*RPlaySoundUPP)(MADDriverRecPtr theRec, char *, long, int, int, int, long, long, unsigned int, Boolean);

#pragma pack(push, 2)

typedef struct Pcmd {
	/// Number of tracks in \c myCmd[]
	short	tracks;
	/// Number of rows in \c myCmd[]
	short	length;
	/// Track ID of first track in \c myCmd[]
	short	trackStart;
	/// Row ID of first row in \c myCmd[]
	short	posStart;
	/// Struct size in bytes - see Definition
	int		structSize;
	Cmd		myCmd[];
} Pcmd;

/// defined because Swift is a dumb-dumb about
/// declared variable-sized c arrays.
typedef struct IntPcmd {
	/// Number of tracks in \c myCmd
	short	tracks;
	/// Number of rows in \c myCmd
	short	length;
	/// Track ID of first track in \c myCmd
	short	trackStart;
	/// Row ID of first row in \c myCmd
	short	posStart;
	/// Count of Cmds in array
	int		cmdCount;
	Cmd		*myCmd;
} IntPcmd;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

/// Deprecated, do not use.
PPEXPORT OSErr inAddSoundToMAD(void			*theSound,
							   size_t		sndLen,
							   long			lS,
							   long			lE,
							   short		sS,
							   short		bFreq,
							   unsigned int	rate,
							   bool			stereo,
							   Str255		name,
							   InstrData	*InsHeader,					// Ptr on instrument header
							   sData		**sample,					// Ptr on samples data
							   short		*sampleID) DEPRECATED_ATTRIBUTE;

PPEXPORT OSErr MADAddSoundToMAD(void			*theSound,
								size_t			sndLen,
								int				loopStart,
								int				loopEnd,
								short			sS,
								short			bFreq,
								unsigned int	rate,
								bool			stereo,
								char			*name,
								InstrData		*InsHeader,				// Ptr on instrument header
								sData			**sample,				// Ptr on samples data
								short			*sampleID);

PPEXPORT sData *MADCreateSampleRaw();
PPEXPORT void ConvertInstrumentIn(Byte *tempPtr, size_t sSize);
PPEXPORT void ConvertInstrumentIn16(short *tempPtr, size_t sSize);
PPEXPORT Cmd* MADGetCmd(short row, short track, Pcmd* myPcmd);

PPEXPORT const CFStringRef kMadPlugMenuNameKey;
PPEXPORT const CFStringRef kMadPlugAuthorNameKey;
PPEXPORT const CFStringRef kMadPlugUTITypesKey;
PPEXPORT const CFStringRef kMadPlugTypeKey;
PPEXPORT const CFStringRef kMadPlugDoesImport;
PPEXPORT const CFStringRef kMadPlugDoesExport;
PPEXPORT const CFStringRef kMadPlugModeKey;

#pragma mark Swift helpers

PPEXPORT IntPcmd *MADPcmdToInt(Pcmd *inVal);
PPEXPORT Pcmd *MADIntPcmdToPcmd(IntPcmd *inVal, bool freeIntPcmd);
PPEXPORT MADErr MADCopyPcmdToPcmd(Pcmd* toCopy, Pcmd** outCopy);
PPEXPORT void MADFreeIntPcmd(IntPcmd *toDelete);

#ifdef __cplusplus
}
#endif

typedef struct PPInfoPlug {
	RPlaySoundUPP	RPlaySound;
	MADDriverRecPtr	driverRec;
	OSType			fileType;
	CFStringRef		fileUTI;
} PPInfoPlug;

#pragma mark Filters for Samples/Sounds
/******************************************************************/
//	*****************	FILTERS FOR SAMPLES/SOUNDS	***************/
//
//	Your main function have to be in this form:
//	OSErr main(sData					*theData,					// Sample informations, see MAD.h
//					long					SelectionStart,				// SelectionStart
//					long					SelectionEnd,				// SelectionEnd, your filter SHOULD apply his effect only on the selection
//					PPInfoPlug				*thePPInfoPlug				// Some functions of PlayerPRO that you can use in your plugs
//					short					stereoMode)					// StereoMode, see 'Silence.c' example
//
//	*****************						***********************/
//
//	If you want to reallocate theData or theData->data:
//
//	if (theData->data != NULL) free(theData->data);			// VERY IMPORTANT to free memory
//	theData->data = malloc(newsize);						// Use malloc ONLY to allocate memory!
//
//	theData->size = newsize;								// In bytes !! Even for 16 bits !
//
//	Don't forget to UPDATE the theData->size !!!!!!!!!!!!
//
/********************						***********************/

#pragma mark Instruments Import/Export Plugs

/******************************************************************/
//******************* INSTRUMENTS IMPORT/EXPORT PLUGS  ************/
//
//	Your main function have to be in this form:
//	OSErr main(OSType					order,			// Order to execute
//					InstrData				*InsHeader,		// Ptr on instrument header
//					sData					**sample,		// Ptr on samples data
//					short					*sampleID,		// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
//															// If sampleID == -1 : add sample else replace selected sample.
//					FSSpec					*AlienFile,		// IN/OUT file
//					PPInfoPlug				*thePPInfoPlug)	// Some functions of PlayerPRO that you can use in your plugs
//
//
//	*****************						***********************/
//	Actual plug have to support these orders:
//
//	order: 'TEST':	check the AlienFile to see if your Plug really supports it.
//	order: 'IMPL':	convert the AlienFile into a PlayerPRO instrument. You have to allocate/dispose your sData*. NOT InsHeader!
//	order: 'EXPL':	Convert current instrument&samples into a file.
//	order: 'PLAY':	Play the sound file at base note via PlayerPRO driver in SYNC.
//	*****************						***********************/
//
/********************						***********************/

// FD7154D6-20BF-4007-881B-8E44970C3B0A
#define kPlayerPROInstrumentPlugTypeID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xFD, 0x71, 0x54, 0xD6, 0x20, 0xBF, 0x40, 0x07, 0x88, 0x1B, 0x8E, 0x44, 0x97, 0x0C, 0x3B, 0x0A)

// 8DC7C582-1C4B-4F3C-BEC8-05CF8323CEA4
#define kPlayerPROInstrumentPlugInterfaceID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x8D, 0xC7, 0xC5, 0x82, 0x1C, 0x4B, 0x4F, 0x3C, 0xBE, 0xC8, 0x05, 0xCF, 0x83, 0x23, 0xCE, 0xA4)

typedef struct PPInstrumentPlugin {
	IUNKNOWN_C_GUTS;
	MADErr (STDMETHODCALLTYPE *InstrumentMain)(void* thisInterface, OSType, InstrData*, sData**, short*, CFURLRef, PPInfoPlug*);
} PPInstrumentPlugin;

#pragma mark Digital Editor Plugs

/******************************************************************/
//******************* DIGITAL EDITOR PLUGS  ***********************/
//
//	Your main function have to be in this form:
//	OSErr main(	Pcmd					*Pcmd,						// Digital Selection
//				PPInfoPlug				*thePPInfoPlug)				// Some functions of PlayerPRO that you can use in your plugs
//
//
//	*****************						***********************/
//
//	If you want to reallocate Pcmd:
//
//	if (Pcmd != NULL) free(Pcmd);							// VERY IMPORTANT
//	Pcmd = malloc sizeof(Pcmd) + noCell * sizeof(Cmd));		// Use malloc ONLY to allocate memory!
//
//	myPcmd->structSize 	= sizeof(Pcmd) + noCell * sizeof(Cmd);
//
//	Don't forget to UPDATE the myPcmd->structSize !!!!!!!!!!!!
//
//	*****************						***********************/
//
//
/********************						***********************/

#endif
