/*	SND				*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

//	Usage:

#include <Carbon/Carbon.h>
#include <PlayerPROCore/PlayerPROCore.h>

//Taken from OS X's Kerberos source code file FSpUtils.c, as found ot:
// http://www.opensource.apple.com/source/Kerberos/Kerberos-47/KerberosFramework/Common/Sources/FSpUtils.c
//Note that the URL might change.
static OSStatus CFURLToFSSpec (CFURLRef pathURL, FSSpec *outSpec)
{
	OSStatus err = noErr;
	FSRef ref;
	FSCatalogInfo info;
	CFStringRef pathString = NULL;
	CFURLRef parentURL = NULL;
	CFStringRef nameString = NULL;
	
	// First, try to create an FSRef for the full path
	if (err == noErr) {
		if (CFURLGetFSRef(pathURL, &ref)) {
			err = FSIsFSRefValid(&ref) ? noErr : fnfErr;
		} else {
			err = fnfErr;
		}
	}
	
	if (err == noErr) {
		// It's a directory or a file that exists; convert directly into an FSSpec:
		err = FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, NULL, outSpec, NULL);
	} else {
		Str255 fileName;
		// The suck case.  The file doesn't exist.
		err = noErr;
		
		// Get a CFString for the path
		if (err == noErr) {
			pathString = CFURLCopyFileSystemPath(pathURL, kCFURLPOSIXPathStyle);
			if (pathString == NULL)
				err = memFullErr;
		}
		
		// Get a CFURL for the parent
		if (err == noErr) {
			parentURL = CFURLCreateCopyDeletingLastPathComponent(kCFAllocatorDefault, pathURL);
			if (parentURL == NULL)
				err = memFullErr;
		}
		
		// Build an FSRef for the parent directory, which must be valid to make an FSSpec
		if (err == noErr) {
			Boolean converted = CFURLGetFSRef(parentURL, &ref);
			if (!converted)
				err = fnfErr;
		}
		
		// Get the node ID of the parent directory
		if (err == noErr)
			err = FSGetCatalogInfo(&ref, kFSCatInfoNodeFlags|kFSCatInfoNodeID|kFSCatInfoVolume, &info, NULL, outSpec, NULL);
		
		// Get a CFString for the file name
		if (err == noErr) {
			nameString = CFURLCopyLastPathComponent(pathURL);
			if (nameString == NULL)
				err = memFullErr;
		}
		
		// Copy the string into the FSSpec
		if (err == noErr) {
			Boolean converted = CFStringGetPascalString(pathString, fileName, sizeof(fileName),
														kCFStringEncodingMacRoman);
			if (!converted)
				converted = CFStringGetPascalString(pathString, fileName, sizeof(fileName),
													CFStringGetSystemEncoding());
			
			if (!converted)
				err = fnfErr;
		}
		
		// Set the node ID in the FSSpec
		if (err == noErr)
			err = FSMakeFSSpec(info.volume, info.nodeID, fileName, outSpec);
	}
	
	// Free allocated memory
	if (pathString != NULL)
		CFRelease(pathString);
	if (parentURL != NULL)
		CFRelease(parentURL);
	if (nameString != NULL)
		CFRelease(nameString);
	
	return err;
}

static Ptr NSndToPtr(Ptr soundPtr, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *sampleRate, long *baseFreq, Boolean *stereo)
{
	short 			soundFormat, numChannels;
	short 			numSynths, numCmds, CompressID;
	long 			offset, MusSize;
	SoundHeaderPtr 	header;
	CmpSoundHeader	*CmpHeader;
	ExtSoundHeader	*ExtHeader;
	OSErr 			result;
	long			i;
	
	*loopStart = 0;
	*loopEnd = 0;
	*sampleSize = 8;
	*stereo = false;
	
	// determine what format sound we have.
	soundFormat = *(short*) soundPtr;
	MADBE16(&soundFormat);
	
	switch (soundFormat) {
		case 1:						// format 1 sound.
			// look inside the format 1 resource and deduce offsets.
			numSynths = ((short*)soundPtr)[1];					// get # synths.
			MADBE16(&numSynths);
			numCmds = *(short*)(soundPtr + 4 + numSynths * 6);		// get # commands.
			break;
			
		case 2:						// format 2 sound.
			numSynths = 0;			// format 2 sounds have no synth's.
			numCmds = ((short*)soundPtr)[2];
			break;
			
		default:					// jack says, what about 12? or 6?
			MADDebugStr(__LINE__, __FILE__, "NSndToHandle... Burkk");
			return NULL;
			break;
	}
	MADBE16(&numCmds);
	
	// compute address of sound header.
	offset = 6 + 6*numSynths + 8*numCmds;
	header = (SoundHeaderPtr)(((Ptr)soundPtr) + offset);
	
	switch (header->encode) {
		case cmpSH: {
			CompressionInfo	cp;
			OSType			format;
			
			CmpHeader = (CmpSoundHeader*) header;
			CompressID = CmpHeader->compressionID;
			numChannels = CmpHeader->numChannels;
			format = CmpHeader->format;
			
			*loopStart = CmpHeader->loopStart;
			*loopEnd = CmpHeader->loopEnd;
			*sampleSize = CmpHeader->sampleSize;
			if (numChannels == 2)
				*stereo = true;
			else
				*stereo = false;
			
			if (sampleRate != NULL)
				*sampleRate = CmpHeader->sampleRate;
			if (baseFreq != NULL)
				*baseFreq = CmpHeader->baseFrequency;
			
			MusSize = CmpHeader->numFrames;
			
			result = GetCompressionInfo((*CmpHeader).compressionID, (*CmpHeader).format, numChannels, *sampleSize, &cp);
			if (result != noErr)
				DebugStr("\pGetCompressionInfo");
			
			memmove(soundPtr, CmpHeader->sampleArea, CmpHeader->numFrames * cp.bytesPerFrame);
			
			{
				SoundConverter		sc;
				SoundComponentData	inputFormat, outputFormat;
				unsigned long		inputFrames, outputFrames;
				unsigned long		outputBytes;
				OSErr				err;
				Ptr					dstPtr;
				
				inputFormat.flags = 0;
				inputFormat.format = format;
				inputFormat.numChannels = numChannels;
				inputFormat.sampleSize = *sampleSize;
				inputFormat.sampleRate = *sampleRate;
				inputFormat.sampleCount = 0;
				inputFormat.buffer = nil;
				inputFormat.reserved = 0;
				
				outputFormat = inputFormat;
				if (*sampleSize == 8)
					outputFormat.format = kOffsetBinary;
				else
					outputFormat.format = k16BitBigEndianFormat;
				
				err = SoundConverterOpen(&inputFormat, &outputFormat, &sc);
				if (err != noErr)
					MADDebugStr(__LINE__, __FILE__, "Open failed");
				
				err = SoundConverterBeginConversion(sc);
				if (err != noErr)
					MADDebugStr(__LINE__, __FILE__, "Begin Conversion failed");
				
				inputFrames = MusSize;
				
				dstPtr = NewPtr(inputFrames * numChannels * (*sampleSize/8) * cp.samplesPerPacket);
				if (dstPtr == NULL) {
					DisposePtr(soundPtr);
					return NULL;
				}
				
				err = SoundConverterConvertBuffer(sc, soundPtr, inputFrames, dstPtr, &outputFrames, &outputBytes);
				if (err != noErr)
					MADDebugStr(__LINE__, __FILE__, "Conversion failed");
				
				MusSize = outputBytes;
				
				err = SoundConverterEndConversion(sc, dstPtr, &outputFrames, &outputBytes);
				if (err != noErr)
					MADDebugStr(__LINE__, __FILE__, "End Conversion failed");
				
				if (outputBytes != 0) MADDebugStr(__LINE__, __FILE__, "");
				
				err = SoundConverterClose(sc);
				if (err != noErr)
					MADDebugStr(__LINE__, __FILE__, "Close failed");
				
				DisposePtr(soundPtr);
				soundPtr = dstPtr;
			}
		}
			break;
			
		case extSH:
			ExtHeader = (ExtSoundHeader*) header;
			
			MusSize = ExtHeader->numFrames;
			numChannels = ExtHeader->numChannels;
			
			*loopStart = ExtHeader->loopStart;
			*loopEnd = ExtHeader->loopEnd;
			*sampleSize = ExtHeader->sampleSize;
			
			if (sampleRate != NULL)
				*sampleRate	= ExtHeader->sampleRate;
			if (baseFreq != NULL)
				*baseFreq = ExtHeader->baseFrequency;
			
			if (numChannels == 2)
				*stereo = true;
			else
				*stereo = false;
			
			if (*stereo)
				MusSize *= 2;
			
			if (*sampleSize == 16) {
				MusSize *= 2;
				*loopStart *= 2;
				*loopEnd *= 2;
			}
			
			if (numChannels == 1)
				memmove(soundPtr, ExtHeader->sampleArea, MusSize);
			else if (numChannels == 2)
				memmove(soundPtr, ExtHeader->sampleArea, MusSize);
			else {
				if (*sampleSize == 8) {
					for (i = 0; i < MusSize; i ++) {
						soundPtr[i] = ExtHeader->sampleArea[i * numChannels];
					}
				} else {
					MusSize /= 2;
					for (i = 0; i < MusSize; i ++) {
						((short*) soundPtr)[i] = ((short*) ExtHeader->sampleArea)[i * numChannels];
					}
					MusSize *= 2;
				}
			}
			break;
			
		default:
		case stdSH:
			*loopStart = header->loopStart;
			*loopEnd = header->loopEnd;
			
			if (sampleRate != NULL)
				*sampleRate	= header->sampleRate;
			if (baseFreq != NULL)
				*baseFreq 	= header->baseFrequency;
			
			MusSize = header->length;
			BlockMoveData((*header).sampleArea, soundPtr, MusSize);
			break;
	}
	
	if (*sampleSize == 8) {
		ConvertInstrumentIn((Byte*)soundPtr, MusSize);
	}
	
	SetPtrSize(soundPtr, MusSize);
	
	if (*loopEnd - *loopStart < 4) {
		*loopEnd = 0;
		*loopStart = 0;
	}
	
	return soundPtr;
}

static inline MADErr TestSND(short *soundPtr)
{
	short oldSound = *soundPtr;
	MADBE16(&oldSound);
	if (oldSound == 1 || oldSound == 2)
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static MADErr MacsndMain(void* thisInterface, OSType, InstrData*, sData**, short*, CFURLRef, PPInfoPlug*);

MADErr MacsndMain(void		*thisInterface,
				  OSType	order,				// Order to execute
				  InstrData	*InsHeader,			// Ptr on instrument header
				  sData		**sample,			// Ptr on samples data
				  short		*sampleID,			// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
												// If sampleID == -1 : add sample else replace selected sample.
				  CFURLRef		alienFileURL,	// IN/OUT file
				  PPInfoPlug	*thePPInfoPlug)
{
	OSErr		myErr;
	FSIORefNum	iFileRefI;
	long		inOutBytes;
	FSSpec		ourSpec;
	OSStatus	didConvert = CFURLToFSSpec(alienFileURL, &ourSpec);
#define AlienFileFSSpec (&ourSpec)
	
	if (didConvert != noErr) {
		return MADReadingErr;
	}
	
	switch (order) {
		case 'IMPL':
		{
			Ptr				theSound;
			long			lS, lE, bFreq;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			
			myErr = FSpOpenDF(AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if (myErr == noErr) {
				SInt64 tmpForkSize = 0;
				FSGetForkSize(iFileRefI, &tmpForkSize);
				inOutBytes = (long)tmpForkSize;
				
				theSound = NewPtr(inOutBytes);
				if (theSound == NULL)
					myErr = MADNeedMemory;
				else {
					FSRead(iFileRefI, &inOutBytes, theSound);
					
					theSound = NSndToPtr(theSound, &lS, &lE, &sS, &rate, &bFreq, &stereo);
					
					if (theSound) {
						size_t tmpSize = GetPtrSize(theSound);
						void *tmpSound = malloc(tmpSize);
						memcpy(tmpSound, theSound, tmpSize);
						
						inAddSoundToMAD(tmpSound, tmpSize, lS, lE, sS, bFreq, rate, stereo, AlienFileFSSpec->name, InsHeader, sample, sampleID);
					} else {
						myErr = MADNeedMemory;
					}
					DisposePtr(theSound);
				}
				
				FSCloseFork(iFileRefI);
			}
		}
			break;
			
		case 'TEST':
		{
			Ptr	theSound;
			
			myErr = FSpOpenDF(AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if (myErr == noErr) {
				inOutBytes = 50L;
				theSound = NewPtr(inOutBytes);
				if (theSound == NULL)
					myErr = MADNeedMemory;
				else {
					FSRead(iFileRefI, &inOutBytes, theSound);
					
					myErr = TestSND((short*)theSound);
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

// 96F7A141-84CF-4CEE-9FC9-11BFF33F6626
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x96, 0xF7, 0xA1, 0x41, 0x84, 0xCF, 0x4C, 0xEE, 0x9F, 0xC9, 0x11, 0xBF, 0xF3, 0x3F, 0x66, 0x26)
#define PLUGINFACTORY MacsndFactory
#define PLUGMAIN MacsndMain

#include "../CFPlugin-InstrBridge.c"
