/*	WAV		*/
/*  IMPORT	*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#include <AudioToolbox/AudioToolbox.h>
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

static char *GetFileNameFromCFURL(CFURLRef theRef)
{
	char *fileName;
	CFIndex filenamLen;
	CFRange theDotWav;
	char *FileNameLong = NULL;
	CFStringRef filenam = CFURLCopyLastPathComponent(theRef);
	if (CFStringFindWithOptions(filenam, CFSTR(".wav"), CFRangeMake(0, CFStringGetLength(filenam)), kCFCompareCaseInsensitive | kCFCompareBackwards, &theDotWav)) {
		CFURLRef tmpRef = CFURLCreateCopyDeletingPathExtension(kCFAllocatorDefault, theRef);
		CFRelease(filenam);
		filenam = CFURLCopyLastPathComponent(tmpRef);
		CFRelease(tmpRef);
	}
	filenamLen = CFStringGetMaximumSizeForEncoding(CFStringGetLength(filenam), kCFStringEncodingMacRoman) * 2;
	FileNameLong = malloc(filenamLen);
	if (!CFStringGetCString(filenam, FileNameLong, filenamLen, kCFStringEncodingMacRoman)) {
		free(FileNameLong);
		CFRelease(filenam);
		return NULL;
	}
	CFRelease(filenam);
	fileName = realloc(FileNameLong, strlen(FileNameLong) + 1);
	if (!fileName)
		return FileNameLong;
	
	return fileName;
}

static Boolean AudioStreamNeedsConversion(AudioStreamBasicDescription fromFormat)
{
	if (fromFormat.mFramesPerPacket != 1) {
		return TRUE;
	}
	if (fromFormat.mFormatID != kAudioFormatLinearPCM) {
		return TRUE;
	}
	if (fromFormat.mFormatFlags != (kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked)) {
		return TRUE;
	}
	if (fromFormat.mSampleRate < 5000 || fromFormat.mSampleRate > 48000 || fromFormat.mSampleRate != floor(fromFormat.mSampleRate)) {
		return TRUE;
	}
	if (fromFormat.mBitsPerChannel != 16 && fromFormat.mBitsPerChannel != 8) {
		return TRUE;
	}
	if (fromFormat.mChannelsPerFrame != 1 && fromFormat.mChannelsPerFrame != 2) {
		return TRUE;
	}
	
	return FALSE;
}

static AudioStreamBasicDescription GetBestApproximationFromAudioStream(AudioStreamBasicDescription fromFormat)
{
	AudioStreamBasicDescription toFormat = {0};
	toFormat.mFramesPerPacket = 1;
	toFormat.mFormatID = kAudioFormatLinearPCM;
	toFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
	if (fromFormat.mSampleRate < 5000) {
		toFormat.mSampleRate = 5000;
	} else if(fromFormat.mSampleRate > 48000) {
		toFormat.mSampleRate = 48000;
	} else {
		toFormat.mSampleRate = floor(fromFormat.mSampleRate);
	}
	
	switch (fromFormat.mBitsPerChannel) {
		case 8:
		case 16:
			toFormat.mBitsPerChannel = fromFormat.mBitsPerChannel;
			break;
			
		default:
			toFormat.mBitsPerChannel = 16;
			break;
	}
	
	switch (fromFormat.mChannelsPerFrame) {
		case 1:
		case 2:
			toFormat.mChannelsPerFrame = toFormat.mChannelsPerFrame;
			break;
			
		default:
			toFormat.mChannelsPerFrame = 2;
			break;
	}
	toFormat.mBytesPerFrame = toFormat.mBitsPerChannel * toFormat.mChannelsPerFrame / 8;
	toFormat.mBytesPerPacket = toFormat.mBytesPerFrame * toFormat.mFramesPerPacket;
	
	return toFormat;
}

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
	AudioFileID audioFile;
	OSStatus res = noErr;
	
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
				
				inAddSoundToMADCString(theSound, sndLen, (int)lS, (int)lE, sS, 60, rate, stereo, shortName, InsHeader, sample, sampleID);
				free(shortName);
			}
		}
			break;
			
		case MADPlugTest:
			res = AudioFileOpenURL(AlienFileRef, kAudioFileReadPermission, kAudioFileWAVEType, &audioFile);
			if (res != noErr) {
				myErr = MADFileNotSupportedByThisPlug;
			} else {
				AudioFileClose(audioFile);
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

