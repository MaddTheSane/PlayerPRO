//
//  PPDriver.m
//  PPMacho
//
//  Created by C.W. Betts on 11/28/12.
//
//

#import "PPDriver.h"
#import "PPLibrary.h"
#import "PPLibrary_PPKPrivate.h"
#import "PPDriver_Private.h"
#import "PPMusicObject.h"
#import "PPMusicObject_PPKPrivate.h"
#include <PlayerPROCore/RDriverInt.h>

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

- (NSTimeInterval)currentMusicPlaybackTime
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

- (int)availableChannel
{
	return 0; //TODO:implement!
}

- (MADErr)changeDriverSettingsToSettings:(MADDriverSettings)theSett
{
	MADErr theErr;
	[self willChangeValueForKey:@"driverSettings"];
	theErr = MADChangeDriverSettings(&theSett, &theRec);
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

- (void)cleanDriver
{
	MADCleanDriver(theRec);
}
	
- (BOOL)directSaveToPointer:(void*)thePtr settings:(MADDriverSettings*)theSett
{
	return DirectSave(thePtr, theSett, theRec);
}

- (BOOL)isPaused
{
	return !MADIsPlayingMusic(theRec);
}

- (BOOL)isPlayingMusic
{
	return MADIsPlayingMusic(theRec);
}

- (BOOL)isDonePlaying
{
	return [self isDonePlayingMusic];
}

- (BOOL)isDonePlayingMusic
{
	return MADIsDonePlaying(theRec);
}

- (NSInteger)audioLength
{
	return [self audioDataLength];
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

- (instancetype)initWithLibrary:(PPLibrary *)theLib
{
	MADDriverSettings theSet = {0};
	MADGetBestDriver(&theSet);
	return [self initWithLibrary:theLib settings:&theSet error:NULL];
}

- (instancetype)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings
{
	return self = [self initWithLibrary:theLib settings:theSettings error:NULL];
}

- (instancetype)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings error:(out MADErr*)theErr
{
	if (self = [super init]) {
		if (theErr)
			*theErr = MADNoErr;
		
		thePPLib = theLib;
		MADErr iErr = MADCreateDriver(theSettings, theLib._madLib, &theRec);
		if (iErr != MADNoErr) {
			if (theErr)
				*theErr = iErr;
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
	PPMusicObject *theMus = [[PPMusicObject alloc] initWithPath:path library:self.theLibrary];
	if (theMus)
		[theMus attachToDriver:self];
	
	return theMus;
}

- (PPMusicObject *)loadMusicURL:(NSURL*)url
{
	PPMusicObject *theMus = [[PPMusicObject alloc] initWithURL:url library:self.theLibrary];
	if (theMus)
		[theMus attachToDriver:self];
	
	return theMus;
}

@end
