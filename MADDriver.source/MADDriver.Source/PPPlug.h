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

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#else
#if !defined(THINK_C)
#pragma options align=mac68k
#endif
#endif

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
//	if( theData->data != 0L) DisposPtr( theData->data);		// VERY IMPORTANT to free memory
//	theData->data = NewPtr( newsize);						// Use NewPtr ONLY to allocate memory!
//	
//	theData->size = newsize;								// In bytes !! Even for 16 bits !
//	
//	Don't forget to UPDATE the theData->size !!!!!!!!!!!!
//
//	*****************						***********************/
//
//	About Resources:
//
//	Your Plug should have: Creator: 'SNPL', Type: 'PLug'
//
//	Your Plug have to have these resources:
//
//	- One resource CODE 1000 with 68k Code  ** You should NOT use the 68881 coprocessor **
//	- One resource PPCC 1000 with PPC Code  (OPTIONAL: if PlayerPRO PPC version cannot find it, it will use the CODE 1000 68k resource)
//	- One STR# resource :
//
//		1 string: Menu Name (see Instrument window in PlayerPRO)
//
/********************						***********************/

typedef struct
{
	void		*RPlaySoundUPP;			//	OSErr			RPlaySound( Ptr whichSound, long SoundSize, long whichTrack, long Period, long Amplitude, long loopStart, long loopLength, Boolean Stereo?)
	void		*UpdateALLWindowUPP;	//	void			UpdateALLWindow( void)
	void		*MyDlgFilterUPP;		//	pascal Boolean	MyDlgFilter( DialogPtr theDlg, EventRecord *theEvt, short *itemHit)
	OSType		fileType;
	
} PPInfoPlug;

typedef OSErr			(*RPlaySoundUPP)		( Ptr, long, long, long, long, long, long, unsigned long, Boolean);
typedef void			(*UpdateALLWindowUPP)	( void);
typedef pascal Boolean	(*MyDlgFilterUPP)		( DialogPtr, EventRecord*, short*);


/*#if defined(powerc) || defined(__powerc)

#include "mixedmode.h"

		// ***** POWERPC calls *********

#define 		RPlaySoundCallMode (	kCStackBased|\
				RESULT_SIZE( SIZE_CODE( sizeof(OSErr) ))|\
				STACK_ROUTINE_PARAMETER( 1, SIZE_CODE( sizeof( Ptr)))|\
				STACK_ROUTINE_PARAMETER( 2, SIZE_CODE( sizeof( long)))|\
				STACK_ROUTINE_PARAMETER( 3, SIZE_CODE( sizeof( long)))|\
				STACK_ROUTINE_PARAMETER( 4, SIZE_CODE( sizeof( long)))|\
				STACK_ROUTINE_PARAMETER( 5, SIZE_CODE( sizeof( long)))|\
				STACK_ROUTINE_PARAMETER( 6, SIZE_CODE( sizeof( long)))|\
 				STACK_ROUTINE_PARAMETER( 7, SIZE_CODE( sizeof( long)))|\
 				STACK_ROUTINE_PARAMETER( 8, SIZE_CODE( sizeof( unsigned long)))|\
 				STACK_ROUTINE_PARAMETER( 9, SIZE_CODE( sizeof( Boolean))))
 				
#define CallRPlaySoundUPP( v1, v2, v3, v4, v5, v6, v7, v8, v9)		\
		CallUniversalProc( thePPInfoPlug->RPlaySoundUPP, RPlaySoundCallMode, v1, v2, v3, v4, v5, v6, v7, v8, v9)

//

#define UpdateALLWindowCallMode (	kCStackBased)

#define CallUpdateALLWindowUPP()		\
		CallUniversalProc( thePPInfoPlug->UpdateALLWindowUPP, UpdateALLWindowCallMode)

//

#else*/	// ******* 68K calls ***********

#define CallRPlaySoundUPP( v1, v2, v3, v4, v5, v6, v7, v8, v9)		\
		(* (RPlaySoundUPP) (thePPInfoPlug->RPlaySoundUPP))( v1, v2, v3, v4, v5, v6, v7, v8, v9)

//

#define CallUpdateALLWindowUPP()		\
		(* (UpdateALLWindowUPP) (thePPInfoPlug->UpdateALLWindowUPP))

//
//#endif

/********************						***********************/
//
//
// RPlaySoundUPP	: Play Sound ( Ptr rawSound, long SoundSize, long whichTrack, long Period, long Amplitude, long loopStart, long loopLength, Boolean Stereo?)
// UpdateALLWindow	: Check all PlayerPRO windows and update them if need it.
// MyDlgFilterUPP	: to use with a ModalDialog function: allow movable dialog, PlayerPRO windows updating, Item 1 Frame, Copy/Paste support, Key support
//
//
/********************						***********************/


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
//	if( Pcmd != 0L) DisposPtr( (Ptr) Pcmd);							// VERY IMPORTANT
//	Pcmd = NewPtrClear( sizeof( Pcmd) + noCell * sizeof( Cmd));		// Use NewPtr ONLY to allocate memory!
//
//	myPcmd->structSize 	= sizeof( Pcmd) + noCell * sizeof( Cmd);
//	
//	Don't forget to UPDATE the myPcmd->structSize !!!!!!!!!!!!
//
//	*****************						***********************/
//
//	About Resources:
//
//	Your Plug should have: Creator: 'SNPL', Type: 'PPDG'
//
//	Your Plug have to have these resources:
//
//	- One resource CODE 1000 with 68k Code  ** You should NOT use the 68881 coprocessor **
//	- One resource PPCC 1000 with PPC Code  (OPTIONAL: if PlayerPRO in PPC cannot find it, it will use the CODE 1000 resource)
//	- One STR# resource :
//
//		1 string: Menu Name (see Button in Digital Editor window in PlayerPRO)
//
/********************						***********************/

typedef struct
{
	short			tracks;					// number of tracks in myCmd[]
	short			length;					// number of rows in myCmd[]
	short			trackStart;				// track ID of first track in myCmd[]
	short			posStart;				// row ID of first row in myCmd[]
	long			structSize;				// struct size in bytes - see Definition
	Cmd				myCmd[];
} Pcmd;

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
//	order: 'IMPT':	convert the AlienFile into a PlayerPRO instrument. You have to allocate/dispose your sData*. NOT InsHeader!
//	order: 'EXPT':	Convert current instrument&samples into a file.
//	order: 'PLAY':	Play the sound file at base note via PlayerPRO driver in SYNC.
//	*****************						***********************/
//
//	About Resources:
//
//	Your Plug should have: Creator: 'SNPL', Type: 'PPIN'
//
//	Your Plug have to have these resources:
//
//	- One resource CODE 1000 with 68k Code  ** You should NOT use the 68881 coprocessor **
//	- One resource PPCC 1000 with PPC Code  (OPTIONAL: if PlayerPRO in PPC cannot find it, it will use the CODE 1000 resource)
//	- One STR# resource :
//
//
//		1 string: which kind of files your plug support (OSType value !!! 4 char) By example: 'WAVE', 'snd ', 'AIFF', etc...
//		2 string: what does your Plug: EXPL : only Export files, IMPL : only Import Files, EXIM : import AND export.
//		3 string: string that will be used in Import and Export menu of PlayerPRO
//		4 string: Copyright string of this plug.
//		5 string: Is it a sample or an instrument format? 'INST' or 'SAMP'
//
/********************						***********************/

OSErr	PPINImportFile( OSType	kindFile, short ins, short *samp, FSSpec	*AlienFile);
OSErr	PPINTestFile( OSType	kindFile, FSSpec	*AlienFile);
OSErr	PPINExportFile( OSType	kindFile, short ins, short samp, FSSpec	*AlienFile);
OSType	PressPPINMenu( Rect	*PopUpRect, OSType curType, short, Str255);
OSErr	PPINAvailablePlug( OSType	kindFile, OSType *plugType); // plugType == 'INST' or 'SAMP'
OSErr	PPINGetPlugByID( OSType *type, short id, short samp);


// SndUtils.c Definition :

Ptr ConvertWAV(FSSpec *fileSpec, long *loopStart, long *loopEnd, short	*sampleSize, unsigned long *rate, Boolean *stereo);
OSErr ConvertDataToWAVE( FSSpec file, FSSpec *newfile, PPInfoPlug *thePPInfoPlug);
void pStrcpy(register unsigned char *s1, register unsigned char *s2);
Ptr 	MyExp1to6( Ptr sound, unsigned long numSampleFrames);
Ptr 	MyExp1to3( Ptr sound, unsigned long numSampleFrames);
void 	ConvertInstrumentIn( register	Byte	*tempPtr,	register long sSize);
OSErr inAddSoundToMAD(	Ptr				theSound,
						long			lS,
						long			lE,
						short			sS,
						short			bFreq,
						unsigned long	rate,
						Boolean			stereo,
						Str255			name,
						InstrData		*InsHeader,					// Ptr on instrument header
						sData			**sample,					// Ptr on samples data
						short			*sampleID);
sData	* inMADCreateSample();


#if defined(powerc) || defined (__powerc)
#pragma options align=reset
#else
#if !defined(THINK_C)
#pragma options align=reset
#endif
#endif
#endif