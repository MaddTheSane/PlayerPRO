/*	SND				*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

//	Usage:

#include <PlayerPROCore/PlayerPROCore.h>
#include <Carbon/Carbon.h>

Ptr MyExp1to3(Ptr sound, unsigned long numSampleFrames);
Ptr MyExp1to6(Ptr sound, unsigned long numSampleFrames);

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

static Ptr inNSndToPtr(Ptr soundPtr, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *sampleRate, long *baseFreq, Boolean *stereo)
{
	short 			soundFormat, numChannels;
	short 			numSynths = 1, numCmds, CompressID;
	long 			offset, MusSize;
	SoundHeaderPtr 	header;
	CmpSoundHeader	*CmpHeader;
	ExtSoundHeader	*ExtHeader;
	//SndCommand 		cmd;
	OSErr 			result;
	long i;
	//long			i,x, numFrames;
	//Boolean			change = false;
	//Str255			aStr;
	
	*loopStart = 0;
	*loopEnd = 0;
	*sampleSize = 8;
	*stereo = false;
	
	// determine what format sound we have.
	soundFormat = *(short*) soundPtr;
	
	switch (soundFormat) {
		case 1:						// format 1 sound.
			// look inside the format 1 resource and deduce offsets.
			numSynths = ((short*)soundPtr)[1];					// get # synths.
			numCmds = *(short*)(soundPtr+4+numSynths*6);		// get # commands.
			break;
			
		case 2:						// format 2 sound.
			numSynths = 0;			// format 2 sounds have no synth's.
			numCmds = ((short*)soundPtr)[2];
			break;
			
		default:					// jack says, what about 12? or 6?
			DebugStr("\p NSndToHandle... Burkk");
			break;
	}
	
	// compute address of sound header.
	offset = 6 + 6*numSynths + 8*numCmds;
	header = (SoundHeaderPtr) (((Ptr) soundPtr) + offset);
	
	switch (header->encode) {
		case cmpSH:
		{
			CompressionInfo 		cp;
			OSType					format;
			
			CmpHeader = (CmpSoundHeader*) header;
			CompressID = CmpHeader->compressionID;
			numChannels = CmpHeader->numChannels;
			format = CmpHeader->format;
			
			*loopStart = CmpHeader->loopStart;
			*loopEnd = CmpHeader->loopEnd;
			*sampleSize = CmpHeader->sampleSize;
			if (numChannels == 2) *stereo = true;
			else *stereo = false;
			
			if (sampleRate != NULL)	*sampleRate	= CmpHeader->sampleRate;
			if (baseFreq != NULL) 	*baseFreq 	= CmpHeader->baseFrequency;
			
			MusSize = (*CmpHeader).numFrames;
			
			if (*stereo)
			{
				*loopStart *=2;
				*loopEnd *=2;
			}
			
			result = GetCompressionInfo((*CmpHeader).compressionID, (*CmpHeader).format, numChannels, *sampleSize, &cp);
			if (result != noErr)
				DebugStr("\pGetCompressionInfo");
			
			BlockMoveData((*CmpHeader).sampleArea, soundPtr, (*CmpHeader).numFrames * cp.bytesPerFrame);
			
			{
				SoundConverter			sc;
				SoundComponentData		inputFormat, outputFormat;
				unsigned long			inputFrames, outputFrames;
				unsigned long			outputBytes;
				//Ptr						inputPtr, outputPtr;
				OSErr					err;
				Ptr						dstPtr;
				
				inputFormat.flags = 0;
				inputFormat.format = format;
				inputFormat.numChannels = numChannels;
				inputFormat.sampleSize = *sampleSize;
				inputFormat.sampleRate = *sampleRate;
				inputFormat.sampleCount = 0;
				inputFormat.buffer = nil;
				inputFormat.reserved = 0;
				
				outputFormat = inputFormat;
				if (*sampleSize == 8) outputFormat.format = kOffsetBinary;
				else outputFormat.format = k16BitBigEndianFormat;
				
				err = SoundConverterOpen(&inputFormat, &outputFormat, &sc);
				if (err != noErr)
					DebugStr("\pOpen failed");
				
				err = SoundConverterBeginConversion(sc);
				if (err != noErr)
					DebugStr("\pBegin Conversion failed");
				
				inputFrames = MusSize;
				
				dstPtr = NewPtr(inputFrames * numChannels * (*sampleSize/8) * cp.samplesPerPacket);
				if (dstPtr == NULL)
				{
					DisposePtr(soundPtr);
					return NULL;
				}
				
				err = SoundConverterConvertBuffer(sc, soundPtr, inputFrames, dstPtr, &outputFrames, &outputBytes);
				if (err != noErr)
					DebugStr("\pConversion failed");
				
				MusSize = outputBytes;
				
				err = SoundConverterEndConversion(sc, dstPtr, &outputFrames, &outputBytes);
				if (err != noErr)
					DebugStr("\pEnd Conversion failed");
				
				if (outputBytes != 0) Debugger();
				
				err = SoundConverterClose(sc);
				if (err != noErr)
					DebugStr("\pClose failed");
				
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
			
			if (sampleRate != NULL)	*sampleRate	= ExtHeader->sampleRate;
			if (baseFreq != NULL) 	*baseFreq 	= ExtHeader->baseFrequency;
			
			if (numChannels == 2) *stereo = true;
			else *stereo = false;
			
			if (*stereo)
			{
				MusSize *= 2;
				*loopStart *=2;
				*loopEnd *=2;
			}
			
			if (*sampleSize == 16)
			{
				MusSize *= 2;
				*loopStart *= 2;
				*loopEnd *= 2;
			}
			
			if (numChannels == 1) BlockMoveData(ExtHeader->sampleArea, soundPtr, MusSize);
			else if (numChannels == 2) BlockMoveData(ExtHeader->sampleArea, soundPtr, MusSize);
			else
			{
				if (*sampleSize == 8)
				{
					for (i = 0; i < MusSize; i ++)
					{
						soundPtr[i] = ExtHeader->sampleArea[i * numChannels];
					}
				}
				else
				{
					MusSize /= 2;
					for (i = 0; i < MusSize; i ++)
					{
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
			
			if (sampleRate != NULL) *sampleRate	= header->sampleRate;
			if (baseFreq != NULL) 	*baseFreq 	= header->baseFrequency;
			
			MusSize = header->length;
			BlockMoveData((*header).sampleArea, soundPtr, MusSize);
			break;
	}
	
	if (*sampleSize == 8) {
		ConvertInstrumentIn((Byte*) soundPtr, MusSize);
	}
	
	SetPtrSize(soundPtr, MusSize);
	
	if (*loopEnd - *loopStart < 4) { *loopEnd = 0;	*loopStart = 0;}
	
	return soundPtr;
}

static Ptr IMPL(long *lS, long *lE, long *bFreq, short *sS, unsigned long *rate, FSSpec *AlienFileFSSpec, Boolean *stereo)
{
	Handle			tempHandle;
	Ptr 			theSound = NULL;
	ResFileRefNum	iFileRefI;
	OSErr			myErr = noErr;
	
	iFileRefI = FSpOpenResFile(AlienFileFSSpec, fsCurPerm);
	
	if (ResError()) {
		CloseResFile(iFileRefI);
		myErr = ResError();
		goto End;
	}
	
	UseResFile(iFileRefI);
	
	if (Count1Resources('snd ') == 0) {
		CloseResFile(iFileRefI);
		myErr = ResError();
		goto End;
	}
	
	tempHandle = Get1IndResource('snd ', 1);
	if (tempHandle != NULL)
	{
		DetachResource(tempHandle);
		
		theSound = NewPtr(GetHandleSize(tempHandle));
		if (theSound != NULL) {
			HLock(tempHandle);
			BlockMoveData(*tempHandle, theSound, GetHandleSize(tempHandle));
			HUnlock(tempHandle);
			
			theSound = inNSndToPtr(theSound, lS, lE, sS, rate, bFreq, stereo);
			
			if (theSound == NULL) {
				DisposeHandle(tempHandle);
				CloseResFile(iFileRefI);
				myErr = MADNeedMemory;
				goto End;
			}
		} else {
			DisposeHandle(tempHandle);
			CloseResFile(iFileRefI);
			myErr = MADNeedMemory;
			goto End;
		}
		
		DisposeHandle(tempHandle);
		tempHandle = NULL;
	} else {
		CloseResFile(iFileRefI);
		myErr = MADNeedMemory;
		goto End;
	}
	CloseResFile(iFileRefI);
	
End:;
	
	if (myErr != noErr)
		return NULL;
	else
		return theSound;
}

static MADErr Sys7Main(void* thisInterface, OSType order, InstrData* InsHeader, sData** sample, short* sampleID, CFURLRef alienFileURL, PPInfoPlug* thePPInfoPlug)
{
	OSErr			myErr = noErr;
	//Ptr			AlienFile;
	ResFileRefNum	iFileRefI;
	//long			inOutBytes;
	FSSpec			ourSpec;
	OSStatus		didConvert = CFURLToFSSpec(alienFileURL, &ourSpec);
#define AlienFileFSSpec (&ourSpec)
	
	if (didConvert != noErr) {
		return MADReadingErr;
	}
	
	switch(order) {
#if 0
		case MADPlugPlay:
		{
			long			lS, lE, bFreq;
			short			sS;
			unsigned long	rate;
			Ptr				theSound;
			Boolean			stereo;
			
			theSound = IMPL(&lS, &lE, &bFreq, &sS, &rate, AlienFileFSSpec, &stereo);
			
			if (theSound != 0L)
			{
				myErr = CallRPlaySoundUPP(theSound, GetPtrSize(theSound), 0, 0xFF, sS, lS, lE, rate, stereo);
				
				DisposePtr(theSound);
				theSound = 0L;
			}
		}
			break;
#endif
			
		case MADPlugImport:
		{
			long			lS, lE, bFreq;
			short			sS;
			unsigned long	rate;
			Ptr				theSound;
			Boolean			stereo;
			
			theSound = IMPL(&lS, &lE, &bFreq, &sS, &rate, AlienFileFSSpec, &stereo);
			
			if (theSound != NULL) {
				size_t soundSize = GetPtrSize(theSound);
				void *theCSound = malloc(soundSize);
				memcpy(theCSound, theSound, soundSize);
				inAddSoundToMAD(theCSound, soundSize, lS, lE, sS, bFreq, rate, stereo, AlienFileFSSpec->name, InsHeader, sample, sampleID);
				myErr = noErr;
				DisposePtr(theSound);
			} else
				myErr = MADNeedMemory;
		}
			break;
			
		case MADPlugTest:
			myErr = noErr;
			
			iFileRefI = FSpOpenResFile(AlienFileFSSpec, fsCurPerm);
			
			if (ResError()) {
				CloseResFile(iFileRefI);
				myErr = ResError();
				goto End;
			}
			
			UseResFile(iFileRefI);
			
			if (Count1Resources('snd ') == 0)
				myErr = MADFileNotSupportedByThisPlug;
			
			CloseResFile(iFileRefI);
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
End:
	
	return myErr;
}

// 94F2BEA7-89F4-440D-99BF-065198DB088B
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x94, 0xF2, 0xBE, 0xA7, 0x89, 0xF4, 0x44, 0x0D, 0x99, 0xBF, 0x06, 0x51, 0x98, 0xDB, 0x08, 0x8B)
#define PLUGINFACTORY Sys7Factory
#define PLUGMAIN Sys7Main

//#include "../CFPlugin-InstrBridge.c"
#include "../CFPlugin-InstrBridge.c"
