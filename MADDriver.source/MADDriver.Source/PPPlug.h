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

#ifndef __RDRIVERH__
#include "RDriver.h"
#endif

#include <CoreFoundation/CFPlugInCOM.h>


typedef OSErr			(*RPlaySoundUPP)		( Ptr, long, long, long, long, long, long, unsigned long, Boolean);

#ifdef __cplusplus
extern "C" {
#endif
	
PPEXPORT OSErr inAddSoundToMAD(Ptr			theSound,
							   size_t		sndLen,
							   long			lS,
							   long			lE,
							   short		sS,
							   short		bFreq,
							   unsigned int	rate,
							   Boolean		stereo,
							   Str255		name,
							   InstrData	*InsHeader,					// Ptr on instrument header
							   sData		**sample,					// Ptr on samples data
							   short		*sampleID);

PPEXPORT sData* inMADCreateSample();

#ifdef __cplusplus
}
#endif


typedef struct
{
	RPlaySoundUPP RPlaySound;
	OSType fileType;
} PPInfoPlug;

#pragma mark Filters for Samples/Sounds
/******************************************************************/
//	*****************	FILTERS FOR SAMPLES/SOUNDS	***************/
//
//	Your main function have to be in this form:
//	OSErr main( 	sData					*theData,					// Sample informations, see MAD.h
//					long					SelectionStart,				// SelectionStart
//					long					SelectionEnd,				// SelectionEnd, your filter SHOULD apply his effect only on the selection
//					PPInfoPlug				*thePPInfoPlug				// Some functions of PlayerPRO that you can use in your plugs
//					short					stereoMode)					// StereoMode, see 'Silence.c' example
//
//	*****************						***********************/
//
//	If you want to reallocate theData or theData->data:
//
//	if( theData->data != 0L) free( theData->data);		// VERY IMPORTANT to free memory
//	theData->data = malloc( newsize);						// Use malloc ONLY to allocate memory!
//
//	theData->size = newsize;								// In bytes !! Even for 16 bits !
//
//	Don't forget to UPDATE the theData->size !!!!!!!!!!!!
//
/********************						***********************/

// 79EA82AD-5A53-46AF-82A9-4A0685B4588C
#define kPlayerPROFiltersPlugTypeID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x79, 0xEA, 0x82, 0xAD, 0x5A, 0x53, 0x46, 0xAF, 0x82, 0xA9, 0x4A, 0x06, 0x85, 0xB4, 0x58, 0x8C)

// DA7082A2-FEF1-4475-B1A4-35C81ED5DB8F
#define kPlayerPROFiltersPlugInterfaceID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xDA, 0x70, 0x82, 0xA2, 0xFE, 0xF1, 0x44, 0x75, 0xB1, 0xA4, 0x35, 0xC8, 0x1E, 0xD5, 0xDB, 0x8F)


typedef struct _PPFiltersPlugin {
    IUNKNOWN_C_GUTS;
	OSErr (STDMETHODCALLTYPE *FiltersMain) (sData *theData, long SelectionStart, long SelectionEnd, PPInfoPlug *thePPInfoPlug, short stereoMode);
} PPFiltersPlugin;

#pragma mark Instruments Import/Export Plugs

/******************************************************************/
//******************* INSTRUMENTS IMPORT/EXPORT PLUGS  ************/
//
//	Your main function have to be in this form:
//	OSErr main( 	OSType					order,						// Order to execute
//					InstrData				*InsHeader,					// Ptr on instrument header
//					sData					**sample,					// Ptr on samples data
//					short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
//																		// If sampleID == -1 : add sample else replace selected sample.
//					FSSpec					*AlienFile,					// IN/OUT file
//					PPInfoPlug				*thePPInfoPlug)				// Some functions of PlayerPRO that you can use in your plugs
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

typedef struct _PPInstrumentPlugin {
    IUNKNOWN_C_GUTS;
	OSErr (STDMETHODCALLTYPE *InstrMain) (OSType,  InstrData*, sData**, short*, CFURLRef, PPInfoPlug*);
} PPInstrumentPlugin;

#pragma mark Digital Editor Plugs

/******************************************************************/
//******************* DIGITAL EDITOR PLUGS  ***********************/
//
//	Your main function have to be in this form:
//	OSErr main( 	Pcmd					*Pcmd,						// Digital Selection
//					PPInfoPlug				*thePPInfoPlug)				// Some functions of PlayerPRO that you can use in your plugs
//
//
//	*****************						***********************/
//
//	If you want to reallocate Pcmd:
//
//	if( Pcmd != 0L) free( (Ptr) Pcmd);							// VERY IMPORTANT
//	Pcmd = malloc( sizeof( Pcmd) + noCell * sizeof( Cmd));		// Use malloc ONLY to allocate memory!
//
//	myPcmd->structSize 	= sizeof( Pcmd) + noCell * sizeof( Cmd);
//
//	Don't forget to UPDATE the myPcmd->structSize !!!!!!!!!!!!
//
//	*****************						***********************/
//
//
/********************						***********************/

// E9E5574F-50B4-43E0-948D-8B7C80D47261
#define kPlayerPRODigitalPlugTypeID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xE9, 0xE5, 0x57, 0x4F, 0x50, 0xB4, 0x43, 0xE0, 0x94, 0x8D, 0x8B, 0x7C, 0x80, 0xD4, 0x72, 0x61)


// 34BA675D-3ED8-49F9-8D06-28A7436A0E4D
#define kPlayerPRODigitalPlugInterfaceID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x34, 0xBA, 0x67, 0x5D, 0x3E, 0xD8, 0x49, 0xF9, 0x8D, 0x06, 0x28, 0xA7, 0x43, 0x6A, 0x0E, 0x4D)


typedef struct
{
	short			tracks;					// number of tracks in myCmd[]
	short			length;					// number of rows in myCmd[]
	short			trackStart;				// track ID of first track in myCmd[]
	short			posStart;				// row ID of first row in myCmd[]
	SInt32			structSize;				// struct size in bytes - see Definition
	Cmd				myCmd[];
} Pcmd;

typedef struct _PPDigitalPlugin {
    IUNKNOWN_C_GUTS;
	OSErr (STDMETHODCALLTYPE *MyProcPtr) (Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug);
} PPDigitalPlugin;



EXP const CFStringRef kMadPlugMenuNameKey;
EXP const CFStringRef kMadPlugAuthorNameKey;
EXP const CFStringRef kMadPlugUTITypesKey;
EXP const CFStringRef kMadPlugTypeKey;
EXP const CFStringRef kMadPlugDoesImport;
EXP const CFStringRef kMadPlugDoesExport;
EXP const CFStringRef kMadPlugModeKey;


#endif
