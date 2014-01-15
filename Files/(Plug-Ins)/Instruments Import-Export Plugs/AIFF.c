/*	AIFF/AIFC						*/
/*  IMPORT/EXPORT					*/
/*	v 1.0							*/
/*	1996 by ANR						*/
/*  2013 rewrite by Madd the Sane	*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

char *GetFileNameFromCFURL(CFURLRef theRef)
{
	char *fileName;
	CFIndex filenamLen;
	CFRange theDotWav;
	char *FileNameLong = NULL;
	CFStringRef filenam = CFURLCopyLastPathComponent(theRef);
	if (CFStringFindWithOptions(filenam, CFSTR(".wav"), CFRangeMake(0, CFStringGetLength(filenam)), kCFCompareCaseInsensitive | kCFCompareBackwards, &theDotWav)) {
		CFRange withoutDotWav = CFRangeMake(0, CFStringGetLength(filenam));
		withoutDotWav.length -= theDotWav.length;
		CFStringRef shortRef = CFStringCreateWithSubstring(kCFAllocatorDefault, filenam, withoutDotWav);
		CFRelease(filenam);
		filenam = shortRef;
	}
	filenamLen = CFStringGetMaximumSizeForEncoding(CFStringGetLength(filenam), kCFStringEncodingMacRoman);
	filenamLen *= 2;
	FileNameLong = malloc(filenamLen);
	if (!CFStringGetCString(filenam, FileNameLong, filenamLen, kCFStringEncodingMacRoman)) {
		free(FileNameLong);
		CFRelease(filenam);
		return NULL;
	}
	CFRelease(filenam);
	fileName = realloc(FileNameLong, strlen(FileNameLong) + 1);
	if (!fileName)
		fileName = FileNameLong;

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
		case 2:
		case 1:
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

#if 0
static OSErr mainAIFF(void					*unused,
					  OSType				order,			// Order to execute
					  InstrData				*InsHeader,		// Ptr on instrument header
					  sData					**sample,		// Ptr on samples data
					  short					*sampleID,		// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
					  // If sampleID == -1 : add sample else replace selected sample.
					  CFURLRef				AlienFileURL,	// IN/OUT file
					  PPInfoPlug			*thePPInfoPlug)
#endif

static OSErr mainAIFF(void *unused, OSType order, InstrData *InsHeader, sData **sample, short *sampleID, CFURLRef AlienFileURL, PPInfoPlug *thePPInfoPlug)
{
	OSErr		myErr = noErr;
	AudioFileID	audioFile;
	OSStatus	res = noErr;
	char		*fileName = GetFileNameFromCFURL(AlienFileURL);
	
	AudioStreamBasicDescription toFormat = {0};
	AudioStreamBasicDescription fromFormat = {0};
	
	switch(order)
	{
		case MADPlugImport:
			res = AudioFileOpenURL(AlienFileURL, kAudioFileReadPermission, kAudioFileAIFFType, &audioFile);
			if (res != noErr) {
				res = AudioFileOpenURL(AlienFileURL, kAudioFileReadPermission, kAudioFileAIFCType, &audioFile);
				if (res == noErr) {
					AudioConverterRef convRef = NULL;
					UInt32 datSize = sizeof(fromFormat);
					AudioFileGetProperty(audioFile, kAudioFilePropertyDataFormat, &datSize, &fromFormat);
					toFormat = GetBestApproximationFromAudioStream(fromFormat);
					res = AudioConverterNew(&fromFormat, &toFormat, &convRef);
					AudioBufferList *mOutputBufferList = NULL;
					
					AudioConverterFillComplexBuffer(convRef, NULL, NULL, &datSize, mOutputBufferList, NULL);
					
					AudioConverterDispose(convRef);
					AudioFileClose(audioFile);
				} else {
					myErr = MADReadingErr;
				}
			} else {
				UInt32 datSize = sizeof(fromFormat);
				AudioFileGetProperty(audioFile, kAudioFilePropertyDataFormat, &datSize, &fromFormat);
				
				if (AudioStreamNeedsConversion(fromFormat)) {
					AudioConverterRef convRef = NULL;
					toFormat = GetBestApproximationFromAudioStream(fromFormat);
					res = AudioConverterNew(&fromFormat, &toFormat, &convRef);
					
					
					AudioConverterDispose(convRef);
				} else {
					
				}
				
				AudioFileClose(audioFile);
			}
			break;
			
		case MADPlugTest:
			res = AudioFileOpenURL(AlienFileURL, kAudioFileReadPermission, kAudioFileAIFFType, &audioFile);
			if (res != noErr) {
				res = AudioFileOpenURL(AlienFileURL, kAudioFileReadPermission, kAudioFileAIFCType, &audioFile);
				if (res != noErr) {
					myErr = MADFileNotSupportedByThisPlug;
				} else {
					AudioFileClose(audioFile);
				}
			} else {
				AudioFileClose(audioFile);
			}
			break;
			
		case MADPlugExport:
			if (*sampleID >= 0) {
				char* data = NULL;
				sData *curData = sample[*sampleID];
				AudioStreamBasicDescription asbd = {0};
				asbd.mSampleRate = curData->c2spd;
				asbd.mFormatID = kAudioFormatLinearPCM;
				asbd.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked | kAudioFormatFlagIsBigEndian;
				asbd.mBitsPerChannel = curData->amp;
				asbd.mChannelsPerFrame = curData->stereo ? 2 : 1;
				asbd.mFramesPerPacket = 1;
				asbd.mBytesPerFrame = asbd.mBitsPerChannel * asbd.mChannelsPerFrame / 8;
				asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;
				
				AudioFileID audioFile;
				OSStatus res;
				if (curData->amp == 16) {
					data = malloc(curData->size);
					if (!data)
						return MADNeedMemory;
					memcpy(data, curData->data, curData->size);
					
					dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t i) {
						PPBE16(&((short*)data)[i]);
					});
				} else
					data = curData->data;
				
				res = AudioFileCreateWithURL(AlienFileURL, kAudioFileAIFFType, &asbd, kAudioFileFlags_EraseFile, &audioFile);
				if (res != noErr) {
					myErr = MADWritingErr;
				} else {
					UInt32 numBytes = curData->size;
					
					res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, data);
					if (res != noErr) {
						myErr = MADWritingErr;
					} else {
#if 0
						if (curData->loopBeg) {
							AIFFLoop theLoop = {0};
							AudioFileSetUserData(audioFile, 'LOOP', 0, sizeof(theLoop), &theLoop);
						}
						AudioFileOptimize(audioFile);
#endif
						if (curData->amp == 16)
							free(data);
						
						res = AudioFileClose(audioFile);
						if (res != noErr)
							myErr = MADWritingErr;
					}
				}
			}
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	if (fileName) {
		free(fileName);
	}
	
	return myErr;
}

// C4B85FAC-BD58-4661-9004-CBBF84BA4EDD
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xC4, 0xB8, 0x5F, 0xAC, 0xBD, 0x58, 0x46, 0x61, 0x90, 0x04, 0xCB, 0xBF, 0x84, 0xBA, 0x4E, 0xDD)
#define PLUGINFACTORY AIFFFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainAIFF //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"
