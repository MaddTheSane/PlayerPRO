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

@implementation PPDriver
@synthesize rec = theRec;
@synthesize currentMusic;

- (void)setCurrentMusic:(PPMusicObject *)curMusic
{
	if (curMusic != currentMusic) {
		if (currentMusic) {
		}
		currentMusic = curMusic;
		MADAttachDriverToMusic(theRec, currentMusic._currentMusic, NULL);
	}
}

@synthesize theLibrary = thePPLib;

- (void)beginExport
{
	MADBeginExport(theRec);
}

- (void)endExport
{
	MADEndExport(theRec);
}

- (void)cleanDriver
{
	MADCleanDriver(theRec);
}
	
- (BOOL)directSaveToPointer:(void*)thePtr settings:(MADDriverSettings*)theSett
{
	return DirectSave(thePtr, theSett, theRec);
}

- (NSInteger)audioLength
{
	return MADAudioLength(theRec);
}

- (OSErr)play
{
	return MADPlayMusic(theRec);
}

- (OSErr)pause
{
	return MADStopMusic(theRec);
}

- (OSErr)stop
{
	OSErr theErr = MADStopMusic(theRec);
	if (theErr) {
		return theErr;
	}
	return MADSetMusicStatus(theRec, 0, 100, 0);
}

- (id)init
{
	NSAssert(NO, @"PPDriver cannot be inited without a library");
	[self doesNotRecognizeSelector:_cmd];
	return nil;
}

- (id)initWithLibrary:(PPLibrary *)theLib
{
	MADDriverSettings theSet = {0};
	MADGetBestDriver(&theSet);
	return [self initWithLibrary:theLib settings:&theSet];
}

- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings
{
	if (self = [super init]) {
		thePPLib = theLib;
		if (MADCreateDriver(theSettings, theLib._madLib, &theRec) !=noErr)
		{
			return nil;
		}
	}
	return self;
}

- (void)dealloc
{
	if(theRec)
		MADDisposeDriver(theRec);
}

- (void)loadMusicFile:(NSString *)path
{
	PPMusicObject *theMus = [[PPMusicObject alloc] initWithPath:path driver:self];
	theMus = nil;
}

- (void)loadMusicURL:(NSURL*)url
{
	PPMusicObject *theMus = [[PPMusicObject alloc] initWithURL:url driver:self];
	theMus = nil;
}

@end
