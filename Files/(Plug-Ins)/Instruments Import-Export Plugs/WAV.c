/*	WAV		*/
/*  IMPORT	*/

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/PPDefs.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>
#include <Carbon/Carbon.h>
#include "WAV.h"

/*______________________________________________________________________
 | _|_																	|
 |  À  WAVE2SND Project: Balthazar				Started:	12/14/92	|
 | 									 	  Last Modified:	12/21/92	|
 | 									 	 		Release:	12/27/92	|
 | 																		|
 | 	Craig Marciniak														|
 | 	151 83rd Ave, NE #305												|
 | 	Fridley, Mn 55432													|
 | 	612-780-5603														|
 | 						 												|
 | Copyright © 1992 by Craig A. Marciniak          All rights reserved. |
 _______________________________________________________________________*/

#define WAVE_FORMAT_PCM		1
#define kmaxVolume			7
 
#define ksysFailed 			"\pSorry, This software requires system 7.0.0 or greater."

#define kStereoStr 			"\pStereo"
#define kMonoStr 			"\pMonaural"

static OSErr mainWave(OSType					order,						// Order to execute
					  InstrData				*InsHeader,					// Ptr on instrument header
					  sData					**sample,					// Ptr on samples data
					  short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
					  // If sampleID == -1 : add sample else replace selected sample.
					  FSSpec					*AlienFileFSSpec,			// IN/OUT file
					  PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr = noErr;
	short	iFileRefI;
	long	inOutBytes;
	
	switch(order)
	{
#if 0
		case 'PLAY':
		{
			Ptr				theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			
			theSound = ConvertWAV(AlienFileFSSpec, &lS, &lE, &sS, &rate, &stereo);
			
			if (theSound != 0L)
			{
				myErr = CallRPlaySoundUPP(theSound, GetPtrSize(theSound), 0, 0xFF, sS, lS, lE, rate, stereo);
				
				DisposePtr(theSound);
				theSound = 0L;
			}
		}
			break;
#endif
			
		case 'IMPL':
		{
			Ptr				theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			
			theSound = ConvertWAV(AlienFileFSSpec, &lS, &lE, &sS, &rate, &stereo);
			
			if (theSound != NULL)
				inAddSoundToMAD(theSound, lS, lE, sS, 60, rate, stereo, AlienFileFSSpec->name, InsHeader, sample, sampleID);
		}
			break;
			
		case 'TEST':
		{
			Ptr	theSound;
			
			myErr = FSpOpenDF(AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if (myErr == noErr)
			{
				inOutBytes = 100L;
				theSound = NewPtr(inOutBytes);
				if (theSound == NULL) myErr = MADNeedMemory;
				else
				{
					FSRead(iFileRefI, &inOutBytes, theSound);
					
					myErr = TestWAV((PCMWavePtr) theSound);
				}
				
				DisposePtr(theSound);
				
				FSCloseFork(iFileRefI);
			}
		}
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}

// F303D78E-41F6-41EB-B18B-F7FC8B967AD2
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xF3, 0x03, 0xD7, 0x8E, 0x41, 0xF6, 0x41, 0xEB, 0xB1, 0x8B, 0xF7, 0xFC, 0x8B, 0x96, 0x7A, 0xD2)
#define PLUGINFACTORY WaveFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainWave //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"

