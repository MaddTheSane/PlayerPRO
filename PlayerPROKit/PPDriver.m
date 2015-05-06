//
//  PPDriver.m
//  PPMacho
//
//  Created by C.W. Betts on 11/28/12.
//
//

#import <PlayerPROKit/PlayerPROKit-Swift.h>
#import "PPDriver.h"
#import "PPLibrary_PPKPrivate.h"
#import "PPDriver_Private.h"
#import "PPMusicObject.h"
#import "PPMusicObject_PPKPrivate.h"
#include <PlayerPROCore/RDriverInt.h>
#import "PPErrors.h"

@implementation PPDriver
@synthesize rec = theRec;
@synthesize currentMusic;
@synthesize theLibrary = thePPLib;

- (void)setCurrentMusic:(PPMusicObject *)curMusic
{
	if (curMusic != currentMusic) {
		currentMusic = curMusic;
		MADAttachDriverToMusic(theRec, currentMusic._currentMusic, NULL);
	}
}

- (NSTimeInterval)totalMusicPlaybackTime
{
	MADErr iErr;
	long curV, totV;
	NSTimeInterval musPos = 0;
	iErr = MADGetMusicStatus(theRec, &totV, &curV);
	if (iErr)
		return musPos;
	
	musPos = totV / 60.0;
	
	return musPos;
}

- (MADDriverSettings)driverSettings
{
	return MADGetDriverSettings(theRec);
}

- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo
{
	return [self playSoundDataFromPointer:theSnd withSize:sndSize fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:0xFF withLoopStartingAt:0 andLoopLength:0];
}

- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote
{
	return [self playSoundDataFromPointer:theSnd withSize:sndSize fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:theNote withLoopStartingAt:0 andLoopLength:0];
}

- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopInRange:(NSRange)loopRange
{
	if (loopRange.location == NSNotFound) {
		return MADParametersErr;
	}
	return [self playSoundDataFromPointer:theSnd withSize:sndSize fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:0xFF withLoopStartingAt:loopRange.location andLoopLength:loopRange.length];
}

- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopInRange:(NSRange)loopRange
{
	if (loopRange.location == NSNotFound) {
		return MADParametersErr;
	}
	return [self playSoundDataFromPointer:theSnd withSize:sndSize fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:theNote withLoopStartingAt:loopRange.location andLoopLength:loopRange.length];

}

- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen
{
	return [self playSoundDataFromPointer:theSnd withSize:sndSize fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:0xFF withLoopStartingAt:loopStart andLoopLength:loopLen];
}

- (MADErr)playSoundDataFromPointer:(const void*)theSnd withSize:(NSUInteger)sndSize fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen
{
	return MADPlaySoundData(theRec, theSnd, sndSize, theChan, theNote, amp, loopStart, loopLen, rate, stereo);
}

- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo
{
	return [self playSoundDataFromPointer:[theSnd bytes] withSize:[theSnd length] fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:0xFF withLoopStartingAt:0 andLoopLength:0];
}

- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote
{
	return [self playSoundDataFromPointer:[theSnd bytes] withSize:[theSnd length] fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:theNote withLoopStartingAt:0 andLoopLength:0];
}

- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopInRange:(NSRange)loopRange
{
	if (loopRange.location == NSNotFound) {
		return MADParametersErr;
	}
	return [self playSoundDataFromPointer:[theSnd bytes] withSize:[theSnd length] fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:0xFF withLoopStartingAt:loopRange.location andLoopLength:loopRange.length];
}

- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopInRange:(NSRange)loopRange
{
	if (loopRange.location == NSNotFound) {
		return MADParametersErr;
	}
	return [self playSoundDataFromPointer:[theSnd bytes] withSize:[theSnd length] fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:theNote withLoopStartingAt:loopRange.location andLoopLength:loopRange.length];
}

- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen
{
	return [self playSoundDataFromPointer:[theSnd bytes] withSize:[theSnd length] fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:0xFF withLoopStartingAt:loopStart andLoopLength:loopLen];
}

- (MADErr)playSoundDataFromData:(NSData*)theSnd fromChannel:(int)theChan amplitude:(short)amp bitRate:(unsigned int)rate isStereo:(BOOL)stereo withNote:(Byte)theNote withLoopStartingAt:(NSUInteger)loopStart andLoopLength:(NSUInteger)loopLen
{
	return [self playSoundDataFromPointer:[theSnd bytes] withSize:[theSnd length] fromChannel:theChan amplitude:amp bitRate:rate isStereo:stereo withNote:theNote withLoopStartingAt:loopStart andLoopLength:loopLen];
}

- (short)availableChannel
{
	return MADFindAFreeChannel(theRec);
}

- (MADErr)changeDriverSettingsToSettings:(MADDriverSettings*)theSett
{
	MADErr theErr;
	[self willChangeValueForKey:@"driverSettings"];
	theErr = MADChangeDriverSettings(theSett, &theRec);
	[self didChangeValueForKey:@"driverSettings"];
	return theErr;
}

- (void)beginExport
{
	[self willChangeValueForKey:@"exporting"];
	MADBeginExport(theRec);
	[self didChangeValueForKey:@"exporting"];
}

- (void)endExport
{
	[self willChangeValueForKey:@"exporting"];
	MADEndExport(theRec);
	[self didChangeValueForKey:@"exporting"];
}

- (BOOL)isExporting
{
	return MADIsExporting(theRec);
}

- (void)setExporting:(BOOL)isExp
{
	if (isExp == MADIsExporting(theRec)) {
		return;
	}
	
	if (isExp)
		MADBeginExport(theRec);
	else
		MADEndExport(theRec);
}

- (MADErr)setMusicStatusWithCurrentTime:(long)curTime maximumTime:(long)maxV minimumTime:(long)minV
{
	return MADSetMusicStatus(theRec, minV, maxV, curTime);
}

- (MADErr)getMusicStatusWithCurrentTime:(long*)curTime totalTime:(long*)totTime
{
	return MADGetMusicStatus(theRec, totTime, curTime);
}

- (NSTimeInterval)musicPosition
{
	MADErr iErr;
	long curV, totV;
	NSTimeInterval musPos = 0;
	iErr = MADGetMusicStatus(theRec, &totV, &curV);
	if (iErr)
		return musPos;
	
	musPos = curV / 60.0;
	
	return musPos;
}

- (void)setMusicPosition:(NSTimeInterval)musicPosition
{
	long curV = musicPosition * 60;
	long tempCurV, tempTotV;
	MADErr iErr = MADGetMusicStatus(theRec, &tempTotV, &tempCurV);
	if (iErr) {
		return; //This will probably be a thrown exception in the future
	}
	
	iErr = MADSetMusicStatus(theRec, 0, tempTotV, curV);
	if (iErr) {
		return; //This will probably be a thrown exception in the future
	}
}

- (NSData*)directSave
{
	size_t aSize = [self audioDataLength];
	MADDriverSettings ourSettings = self.driverSettings;
	if (ourSettings.outPutBits == 16) {
		aSize *= 2;
	} else if (ourSettings.outPutBits == 20 || ourSettings.outPutBits == 24 ) {
		aSize *= 3;
	}
	
	switch (ourSettings.outPutMode) {
		case DeluxeStereoOutPut:
		case StereoOutPut:
			aSize *= 2;
			break;
			
		case PolyPhonic:
			aSize *= 4;
			break;
			
		default:
			break;
	}

	void* thePtr = malloc(aSize);
	NSData *ourData = nil;
	if (MADDirectSave(thePtr, NULL, theRec)) {
		ourData = [[NSData alloc] initWithBytesNoCopy:thePtr length:aSize];
	}
 
	return ourData;
}

- (void)cleanDriver
{
	MADCleanDriver(theRec);
}
	
- (BOOL)directSaveToPointer:(void*)thePtr settings:(MADDriverSettings*)theSett
{
	return MADDirectSave(thePtr, theSett, theRec);
}

- (BOOL)isPaused
{
	return !MADIsPlayingMusic(theRec);
}

- (void)setPaused:(BOOL)paused
{
	if (paused == self.paused) {
		return;
	}
	if (paused) {
		[self pause];
	} else {
		[self play];
	}
}

+ (NSSet*)keyPathsForValuesAffectingPlayingMusic
{
	return [NSSet setWithObjects:@"paused", @"donePlayingMusic", nil];
}

- (BOOL)isPlayingMusic
{
	return MADIsPlayingMusic(theRec);
}

- (BOOL)isDonePlayingMusic
{
	return MADIsDonePlaying(theRec);
}

- (NSInteger)audioDataLength
{
	return MADAudioLength(theRec);
}

- (MADErr)play
{
	[self willChangeValueForKey:@"paused"];
	MADErr iErr = MADStartDriver(theRec);
	if (iErr) {
		[self didChangeValueForKey:@"paused"];
		return iErr;
	}
	
	iErr = MADPlayMusic(theRec);
	[self didChangeValueForKey:@"paused"];
	return iErr;
}

- (MADErr)pause
{
	[self willChangeValueForKey:@"paused"];
	MADErr iErr = MADStopMusic(theRec);
	[self didChangeValueForKey:@"paused"];
	return iErr;
}

- (MADErr)stopDriver
{
	[self willChangeValueForKey:@"paused"];
	MADErr iErr = MADStopDriver(theRec);
	[self didChangeValueForKey:@"paused"];
	return iErr;
}

- (MADErr)stop
{
	[self willChangeValueForKey:@"paused"];
	MADErr theErr = MADStopMusic(theRec);
	if (theErr) {
		[self didChangeValueForKey:@"paused"];
		return theErr;
	}
	MADReset(theRec);
	if (theErr) {
		[self didChangeValueForKey:@"paused"];
		return theErr;
	}
	theErr = MADStopDriver(theRec);
	[self didChangeValueForKey:@"paused"];
	return theErr;
}

- (instancetype)init
{
	NSAssert(NO, @"PPDriver cannot be inited without a library!");
	[self doesNotRecognizeSelector:_cmd];
	return nil;
}

- (instancetype)initWithLibrary:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)theErr
{
	MADDriverSettings theSet = {0};
	MADGetBestDriver(&theSet);
	return [self initWithLibrary:theLib settings:&theSet error:theErr];
}

- (instancetype)initWithLibrary:(PPLibrary *)theLib settings:(inout MADDriverSettings *)theSettings error:(out NSError* __autoreleasing*)theErr
{
	if (self = [super init]) {
		if (theErr)
			*theErr = nil;
		
		thePPLib = theLib;
		MADErr iErr = MADCreateDriver(theSettings, theLib.theLibrary, &theRec);
		if (iErr != MADNoErr) {
			if (theErr)
				*theErr = PPCreateErrorFromMADErrorType(iErr);
			return nil;
		}
	}
	return self;
}

- (void)dealloc
{
	if (theRec)
		MADDisposeDriver(theRec);
}

- (PPMusicObject *)loadMusicFile:(NSString *)path
{
	PPMusicObject *theMus = [[PPMusicObject alloc] initWithPath:path library:self.theLibrary error:nil];
	if (theMus) {
		[theMus attachToDriver:self];
	}
	
	return theMus;
}

- (PPMusicObject *)loadMusicURL:(NSURL*)url
{
	PPMusicObject *theMus = [[PPMusicObject alloc] initWithURL:url library:self.theLibrary error:nil];
	if (theMus) {
		[theMus attachToDriver:self];
	}
	
	return theMus;
}

#pragma mark - More complex methods

#define GetDriverBase() MADDriverGetBase(theRec)

- (MADChannel)channelAtIndex:(NSInteger)idx
{
	NSParameterAssert(idx > MAXTRACK || idx < 0);
	MADDriverBase *drivBase = GetDriverBase();
	
	return drivBase->chan[idx];
}

- (short)patternPosition
{
	MADDriverBase *drivBase = GetDriverBase();

	return drivBase->PartitionReader;
}
- (void)setPatternPosition:(short)patternPosition
{
	NSParameterAssert(patternPosition > MAXPOINTER);
	MADDriverBase *drivBase = GetDriverBase();
	
	drivBase->PartitionReader = patternPosition;
}

- (short)patternIdentifier
{
	MADDriverBase *drivBase = GetDriverBase();
	
	return drivBase->Pat;
}

- (void)setPatternIdentifier:(short)patternIdentifier
{
	MADDriverBase *drivBase = GetDriverBase();
	
	drivBase->Pat = patternIdentifier;
}

- (short)partitionPosition
{
	MADDriverBase *drivBase = GetDriverBase();
	
	return drivBase->PL;
}

- (void)setPartitionPosition:(short)partitionPosition
{
	MADDriverBase *drivBase = GetDriverBase();
	
	drivBase->PL = partitionPosition;
}

- (short)volume
{
	MADDriverBase *drivBase = GetDriverBase();
	
	return drivBase->VolGlobal;
}

- (void)setVolume:(short)volume
{
	NSParameterAssert(volume > 64 || volume < 0);
	MADDriverBase *drivBase = GetDriverBase();
	
	drivBase->VolGlobal = volume;
}

- (BOOL)usesEqualizer
{
	MADDriverBase *drivBase = GetDriverBase();
	
	return drivBase->Equalizer;
}

- (void)setUsesEqualizer:(BOOL)useEqualizer
{
	MADDriverBase *drivBase = GetDriverBase();
	
	drivBase->Equalizer = useEqualizer;
}

- (void*)oscilloscopePointer
{
	MADDriverBase *drivBase = GetDriverBase();
	
	return drivBase->OscilloWavePtr;
}

- (size_t)oscilloscopeSize
{
	MADDriverBase *drivBase = GetDriverBase();
	
	return drivBase->OscilloWaveSize;
}

- (BOOL)isChannelActiveAtIndex:(NSInteger)idx
{
	NSParameterAssert(idx > 255 || idx < 0);
	MADDriverBase *drivBase = GetDriverBase();
	
	return drivBase->Active[idx];
}

- (void)setChannelAtIndex:(NSInteger)idx toActive:(BOOL)enabled
{
	NSParameterAssert(idx > 255 || idx < 0);
	MADDriverBase *drivBase = GetDriverBase();
	
	drivBase->Active[idx] = enabled;
}

@end

@implementation PPDriver (deprecated)

- (BOOL)isDonePlaying
{
	return [self isDonePlayingMusic];
}

- (NSInteger)audioLength
{
	return [self audioDataLength];
}

@end
