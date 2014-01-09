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

- (MADDriverSettings)driverSettings
{
	return MADGetDriverSettings(theRec);
}

- (OSErr)changeDriverSettingsToSettings:(MADDriverSettings)theSett
{
	[self willChangeValueForKey:@"driverSettings"];
	return MADChangeDriverSettings(&theSett, &theRec);
	[self didChangeValueForKey:@"driverSettings"];
}

- (void)beginExport
{
	[self willChangeValueForKey:@"isExporting"];
	MADBeginExport(theRec);
	[self didChangeValueForKey:@"isExporting"];
}

- (void)endExport
{
	[self willChangeValueForKey:@"isExporting"];
	MADEndExport(theRec);
	[self didChangeValueForKey:@"isExporting"];
}

- (BOOL)isExporting
{
	return MADIsExporting(theRec);
}

- (void)setIsExporting:(BOOL)isExp
{
	if (isExp == MADIsExporting(theRec)) {
		return;
	}
	
	[self willChangeValueForKey:@"isExporting"];
	if (isExp)
		MADBeginExport(theRec);
	else
		MADEndExport(theRec);
	[self didChangeValueForKey:@"isExporting"];
}

- (OSErr)getMusicStatusWithCurrentTime:(long*)curTime totalTime:(long*)totTime
{
	return MADGetMusicStatus(theRec, totTime, curTime);
}

- (NSTimeInterval)musicPosition
{
	OSErr iErr;
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
	OSErr iErr = MADGetMusicStatus(theRec, &tempTotV, &tempCurV);
	if (iErr) {
		return; //This will probably be a thrown exception in the future
	}
	[self willChangeValueForKey:@"musicPosition"];
	iErr = MADSetMusicStatus(theRec, 0, tempTotV, curV);
	if (iErr) {
		//return; //This will probably be a thrown exception in the future
	}
	[self didChangeValueForKey:@"musicPosition"];
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

- (OSErr)play
{
	OSErr iErr = MADStartDriver(theRec);
	if (iErr) {
		return iErr;
	}
	
	return MADPlayMusic(theRec);
}

- (OSErr)pause
{
	OSErr iErr = MADStopMusic(theRec);
	if (iErr) {
		return iErr;
	}
	return MADStopDriver(theRec);
}

- (OSErr)stop
{
	OSErr theErr = MADStopMusic(theRec);
	if (theErr) {
		return theErr;
	}
	MADReset(theRec);
	if (theErr) {
		return theErr;
	}
	return MADStopDriver(theRec);
}

- (id)init
{
	NSAssert(NO, @"PPDriver cannot be inited without a library!");
	[self doesNotRecognizeSelector:_cmd];
	return nil;
}

- (id)initWithLibrary:(PPLibrary *)theLib
{
	MADDriverSettings theSet = {0};
	MADGetBestDriver(&theSet);
	return [self initWithLibrary:theLib settings:&theSet error:NULL];
}

- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings
{
	return self = [self initWithLibrary:theLib settings:theSettings error:NULL];
}

- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings error:(out OSErr*)theErr
{
	if (self = [super init]) {
		if (theErr)
			*theErr = noErr;
		
		thePPLib = theLib;
		OSErr iErr = MADCreateDriver(theSettings, theLib._madLib, &theRec);
		if (iErr != noErr) {
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
