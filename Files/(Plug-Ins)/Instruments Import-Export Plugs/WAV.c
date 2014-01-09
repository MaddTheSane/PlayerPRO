/*	WAV		*/
/*  IMPORT	*/

#include <PlayerPROCore/MAD.h>
#include <PlayerPROCore/PPDefs.h>
#include <PlayerPROCore/FileUtils.h>
#include <PlayerPROCore/PPPlug.h>
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

/*___________________ long byte swap for Intel <-> Motorola Conversions*/
//Just going to byteswap on big-endian platforms
/*unsigned long longswap(unsigned long ul)
{
//	return (ul >> 24) | ((ul >> 8) & 0xff00) | ((ul << 8) & 0xff0000) | (ul << 24);
	return EndianU32_LtoN(ul);
}*/
#define longswap(ul) EndianU32_LtoN(ul)

/*___________________ word byte swap for Intel <-> Motorola Conversions*/

/*unsigned short shrtswap(unsigned int us)
{
//	return ((us >> 8) | (us << 8)) & 0xffff;
	return EndianU16_LtoN(us);
}*/
#define shrtswap(us) EndianU16_LtoN(us)

/*_______________________________________________________________________*/

static OSErr mainWave(void					*unused,
					  OSType				order,						// Order to execute
					  InstrData				*InsHeader,					// Ptr on instrument header
					  sData					**sample,					// Ptr on samples data
					  short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
					  // If sampleID == -1 : add sample else replace selected sample.
					  CFURLRef				AlienFileRef,			// IN/OUT file
					  PPInfoPlug			*thePPInfoPlug)
{
	OSErr	myErr = noErr;
		
	switch (order) {
#if 0
		case 'PLAY':
		{
			Ptr				theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			
			theSound = ConvertWAV( AlienFileFSSpec, &lS, &lE, &sS, &rate, &stereo);
			
			if (theSound != 0L)
			{
				myErr = CallRPlaySoundUPP( theSound, GetPtrSize( theSound), 0, 0xFF, sS, lS, lE, rate, stereo);
				
				DisposePtr( theSound);
				theSound = 0L;
			}
		}
			break;
#endif
			
		case MADPlugImport:
		{
			void			*theSound;
			long			lS, lE;
			short			sS;
			unsigned int	rate;
			Boolean			stereo;
			size_t			sndLen;
			
			theSound = ConvertWAVCFURL(AlienFileRef, &sndLen, &lS, &lE, &sS, &rate, &stereo);
			
			if (theSound != NULL) {
				char longName[PATH_MAX] = {0};
				char *shortName = NULL;
				CFStringRef lastPath = CFURLCopyLastPathComponent(AlienFileRef);
				if (CFStringGetCString(lastPath, longName, PATH_MAX, kCFStringEncodingMacRoman)) {
					long thelen = strlen(longName);
					shortName = malloc(++thelen);
					strlcpy(shortName, longName, thelen);
				} else {
					shortName = strdup("Wave Sound");
				}
				CFRelease(lastPath);
				
				inAddSoundToMADCString(theSound, sndLen, lS, lE, sS, 60, rate, stereo, shortName, InsHeader, sample, sampleID);
				free(shortName);
			}
		}
			break;
			
		case MADPlugTest:
		{
			char cPath[PATH_MAX] = {0};
			CFURLGetFileSystemRepresentation(AlienFileRef, true, (unsigned char*)cPath, PATH_MAX);
			UNFILE iFileRef = iFileOpenRead(cPath);
			long inOutBytes = 100L;
			Ptr AlienPtr = malloc(inOutBytes);
			if (AlienPtr == NULL) {
				myErr = MADNeedMemory;
			} else {
				iRead(inOutBytes, AlienPtr, iFileRef);
				myErr = TestWAV((PCMWavePtr)AlienPtr);
				free(AlienPtr);
			}
			iClose(iFileRef);
		}
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
		
	return myErr;
}

// E93F89D6-1126-4F7C-8319-1955A92CC6DD
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xE9, 0x3F, 0x89, 0xD6, 0x11, 0x26, 0x4F, 0x7C, 0x83, 0x19, 0x19, 0x55, 0xA9, 0x2C, 0xC6, 0xDD)
#define PLUGINFACTORY WaveFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainWave //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"

