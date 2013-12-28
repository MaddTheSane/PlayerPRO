
#ifndef PLAYERPRO6
#define PLAYERPRO6 0
#endif

- (OSErr)saveMusicAsAIFFToURL:(NSURL*)theURL usingSettings:(MADDriverSettings*)theSett
{
	NSData *saveData = [self rawBESoundData:theSett];
	if (!saveData) {
		return MADNeedMemory;
	}
#if !PLAYERPRO6
	__block NSError *expErr = nil;
	dispatch_block_t errBlock = ^{
		if (isQuitting) {
			[NSApp replyToApplicationShouldTerminate:YES];
		} else {
			NSRunAlertPanel(@"Export failed", @"Export of the music file failed:\n%@", nil, nil, nil, [expErr localizedDescription]);
		}
	};
	
#define checkError(res) { \
if (res != noErr){ \
if (audioFile != NULL)\
AudioFileClose(audioFile);\
expErr = [NSError errorWithDomain:NSOSStatusErrorDomain code:res userInfo:nil];\
dispatch_async(dispatch_get_main_queue(), errBlock);\
return MADWritingErr; \
} \
}

#else

#define checkError(res) { \
if (res != noErr){ \
if (audioFile != NULL)\
AudioFileClose(audioFile);\
return MADWritingErr; \
} \
}

#endif
	AudioStreamBasicDescription asbd = {0};
	asbd.mSampleRate = theSett->outPutRate;
	asbd.mFormatID = kAudioFormatLinearPCM;
	asbd.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked | kAudioFormatFlagIsBigEndian;
	asbd.mBitsPerChannel = theSett->outPutBits;
	switch (theSett->outPutMode) {
		case MonoOutPut:
			asbd.mChannelsPerFrame = 1;
			break;
			
		default:
		case StereoOutPut:
		case DeluxeStereoOutPut:
			asbd.mChannelsPerFrame = 2;
			break;
			
		case PolyPhonic:
			asbd.mChannelsPerFrame = 4;
			break;
	}
	asbd.mFramesPerPacket = 1;
	asbd.mBytesPerFrame = asbd.mBitsPerChannel * asbd.mChannelsPerFrame / 8;
	asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;
	
	AudioFileID audioFile = NULL;
	OSStatus res;
	res = AudioFileCreateWithURL((__bridge CFURLRef)theURL, kAudioFileAIFFType, &asbd, kAudioFileFlags_EraseFile, &audioFile);
	checkError(res);
	
	UInt32 numBytes = (UInt32)[saveData length];
	
	res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, [saveData bytes]);
	checkError(res);
	
	//TODO: metadata?
	
	res = AudioFileClose(audioFile);
	audioFile = NULL;
	checkError(res);
#undef checkError
	return noErr;
}

