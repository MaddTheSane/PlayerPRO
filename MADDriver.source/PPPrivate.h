/*
 *  PPPrivate.h
 *  PlayerPROCore
 *
 *  Created by C.W. Betts on 6/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __PLAYERPROCORE_PPPRIVATE__
#define __PLAYERPROCORE_PPPRIVATE__

#ifndef __RDRIVERH__
#include "RDriver.h"
#endif

#ifndef PPMacho_MADDriver_h
#include "MADDriver.h"
#endif

#ifndef BUILDINGPPRO
#warning this header should NOT be used outside of PlayerPROCore! The functions and datatypes here may change FOR ANY REASON, including differing compiler includes and defines.
#endif

#ifdef __cplusplus
extern "C" {
#endif
	
/********************						***********************/
/*** 					INTERNAL ROUTINES						***/
/***			    DO NOT use these routines					***/
/********************						***********************/

/*** Deluxe ***/

void	MADCreateOverShoot(MADDriverRec *intDriver);
void	MADKillOverShoot(MADDriverRec *intDriver);
void 	Sampler16AddDeluxe(Channel *curVoice, SInt32 *ASCBuffer, MADDriverRec *intDriver);
void 	Sampler16Addin16Deluxe(Channel *curVoice, SInt32 *ASCBuffer, MADDriverRec *intDriver);
void 	Sample16BufferAddDeluxe(Channel *curVoice, SInt32 *ASCBuffer, MADDriverRec *intDriver);
void 	Play16StereoDeluxe(MADDriverRec *intDriver);
void 	Sampler8in8AddDeluxe(Channel *curVoice, short *ASCBuffer, MADDriverRec *intDriver);
void 	Sampler8in16AddDeluxe(Channel *curVoice, short *ASCBuffer, MADDriverRec *intDriver);
void 	Sample8BufferAddDeluxe(Channel *curVoice, short *ASCBuffer, MADDriverRec *intDriver);
void 	Play8StereoDeluxe(MADDriverRec *intDriver);

/*** Delay ***/

void	Sampler16AddDelay(Channel *curVoice, SInt32 *ASCBuffer, MADDriverRec *intDriver);
void	Sampler16Addin16Delay(Channel *curVoice, SInt32 *ASCBuffer, MADDriverRec *intDriver);
void	Sample16BufferAddDelay(Channel *curVoice, SInt32 *ASCBuffer, MADDriverRec *intDriver);
void	Play16StereoDelay(MADDriverRec *intDriver);
void	Sampler8in8AddDelay(Channel *curVoice, short *ASCBuffer, MADDriverRec *intDriver);
void	Sampler8in16AddDelay(Channel *curVoice, short *ASCBuffer, MADDriverRec *intDriver);
void	Sample8BufferAddDelay(Channel *curVoice, short *ASCBuffer, MADDriverRec *intDriver);
void	Play8StereoDelay(MADDriverRec *intDriver);

/*** 8 Bits ***/

void 	Sampler8in8Add(Channel *curVoice, Ptr ASCBuffer, MADDriverRec *intDriver);
void 	Sampler8in16Add(Channel *curVoice, Ptr ASCBuffer, MADDriverRec *intDriver);
void 	Sampler8in8AddPoly(Channel *curVoice, Ptr ASCBuffer, short chanNo, MADDriverRec *intDriver);
void 	Sampler8in16AddPoly(Channel *curVoice, Ptr ASCBuffer, short chanNo, MADDriverRec *intDriver);
void 	Sample8BufferAdd(Channel *curVoice, Ptr ASCBuffer, MADDriverRec *intDriver);
void 	Sample8BufferAddPoly(Channel *curVoice, Ptr ASCBuffer, short chanNo, MADDriverRec *intDriver);
void	Play8Mono(MADDriverRec *intDriver);
void 	Play8Stereo(MADDriverRec *intDriver);
void 	Play8PolyPhonic(MADDriverRec *intDriver);

/*** 16 Bits ***/

void 	Sampler16Add( Channel *curVoice,  short	*ASCBuffer, MADDriverRec* intDriver);
void 	Sampler16Addin16( Channel *curVoice,  short	*ASCBuffer, MADDriverRec* intDriver);
void 	Sample16BufferAdd( Channel *curVoice,  short	*ASCBuffer, MADDriverRec* intDriver);
void 	Play16Stereo( MADDriverRec* intDriver);
void 	Sampler16AddPoly( Channel *curVoice,  short	*ASCBuffer, short chanNo, MADDriverRec* intDriver);
void 	Sampler16Addin16Poly( Channel *curVoice,  short	*ASCBuffer, short chanNo, MADDriverRec* intDriver);
void 	Sample16BufferAddPoly( Channel *curVoice,  short	*ASCBuffer, short chanNo, MADDriverRec* intDriver);
void 	Play16PolyPhonic( MADDriverRec* intDriver);
void 	Play16Mono( MADDriverRec* intDriver);

/*** 20 Bits ***/
//TODO

/*** 24 Bits ***/
//TODO

/*** Effects ***/

void 	DoEffect( Channel *ch, short call, MADDriverRec *intDriver);
void 	SetUpEffect( Channel *ch, MADDriverRec *intDriver);
void	DoVolCmd( Channel *ch, short call, MADDriverRec *intDriver);

/*** Interruption Functions ***/

void 	NoteAnalyse(MADDriverRec *intDriver);
void 	ReadNote(Channel *curVoice, Cmd *theCommand, MADDriverRec *intDriver);
void 	Filter8Bit(Byte *myPtr, MADDriverRec *intDriver);
void 	Filter8BitX(Byte *myPtr, MADDriverRec *intDriver);
void 	Filter16BitX(short *myPtr, MADDriverRec *intDriver);
void 	BufferCopyM(MADDriverRec *intDriver);
void 	BufferCopyS(MADDriverRec *intDriver);
void 	NoteOff(short oldIns, short oldN, short oldV, MADDriverRec *intDriver);
void 	SampleMIDI(Channel *curVoice, short channel, short curN, MADDriverRec *intDriver);
void 	CleanDriver(MADDriverRec *intDriver);

#ifdef _MAC_H
#pragma mark Core Audio Functions
OSErr	initCoreAudio( MADDriverRec *inMADDriver);
OSErr	closeCoreAudio( MADDriverRec *inMADDriver);
#endif

#ifdef _ESOUND
OSErr initESD(MADDriverRec *inMADDriver);
OSErr closeESD(MADDriverRec *inMADDriver);
#endif

#ifdef WIN32
Boolean	DirectSoundInit(MADDriverRec* driver);
void	DirectSoundClose(MADDriverRec* driver);
Boolean	W95_Init(MADDriverRec* driver);
void	W95_Exit(MADDriverRec* driver);
#endif

#ifdef HAVE_PULSEAUDIO
OSErr initPulseAudio(MADDriverRec *inMADDriver);
OSErr closePulseAudio(MADDriverRec *inMADDriver);
#endif

#ifdef HAVE_PORTAUDIO
OSErr initPortAudio(MADDriverRec *inMADDriver);
OSErr closePortAudio(MADDriverRec *inMADDriver);
#endif

OSErr	CallImportPlug(MADLibrary	*inMADDriver,
					   short		PlugNo,			// CODE du plug
					   OSType		order,
					   char			*AlienFile,
					   MADMusic		*theNewMAD,
					   PPInfoRec	*info);


OSErr	PPTestFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile);
OSErr	PPInfoFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, PPInfoRec	*InfoRec);
OSErr	PPExportFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic *theNewMAD);
OSErr	PPImportFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic **theNewMAD);
OSErr	PPIdentifyFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile);

OSType	GetPPPlugType(MADLibrary *inMADDriver, short ID, OSType type);
void	MInitImportPlug(MADLibrary *inMADDriver, const char*);
void	CloseImportPlug(MADLibrary *inMADDriver);
OSErr	CheckMADFile(char *AlienFile);

#ifndef __PPPLUGH__
PPEXPORT void ConvertInstrumentIn(Byte *tempPtr, size_t sSize);
#endif

#if defined _MAC_H && !TARGET_OS_IPHONE
void SetOSType(CFURLRef, OSType);
#endif

#ifdef __cplusplus
}
#endif

#endif
