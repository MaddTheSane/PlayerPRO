//
//  PPMusicObject.m
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#import "PPMusicObject.h"
#import "PPLibrary.h"
#import "PPLibrary_PPKPrivate.h"
#import "PPDriver.h"
#import "PPDriver_Private.h"
#import "PPMusicObject_PPKPrivate.h"
#include <PlayerPROCore/RDriverInt.h>

@implementation PPMusicObject
@synthesize attachedDriver;
@synthesize _currentMusic = currentMusic;

- (id)initWithURL:(NSURL *)url library:(PPLibrary *)theLib
{
	if (self = [super init]) {
		self.currentLibrary = theLib;
		char type[5];
		CFURLRef tmpURL = CFBridgingRetain(url);
		if (MADMusicIdentifyCFURL(theLib._madLib, type, tmpURL) != noErr) {
			CFRelease(tmpURL);
			return nil;
		}
		if (MADLoadMusicCFURLFile(theLib._madLib, &currentMusic, type, tmpURL) != noErr) {
			CFRelease(tmpURL);
			return nil;
		}
		
		
		CFRelease(tmpURL);
	}
	return self;
}

- (id)initWithPath:(NSString *)url library:(PPLibrary *)theLib
{
	return [self initWithURL:[NSURL fileURLWithPath:url] library:theLib];
}

- (id)init
{
	if (self = [super init]) {
		currentMusic = CreateFreeMADK();
	}
	return self;
}

- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib setAsCurrentMusic:(BOOL)toSet
{
	if (self = [self initWithURL:url library:theLib.theLibrary]) {
		self.attachedDriver = theLib;
		if (toSet) {
			[attachedDriver setCurrentMusic:self];
		}
	}
	return nil;
}

- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib setAsCurrentMusic:(BOOL)toSet
{
	return [self initWithURL:[NSURL fileURLWithPath:url] driver:theLib setAsCurrentMusic:toSet];
}

- (id)initWithURL:(NSURL *)url driver:(PPDriver *)theLib
{
	return [self initWithURL:url driver:theLib setAsCurrentMusic:NO];
}

- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib
{
	return [self initWithPath:url driver:theLib setAsCurrentMusic:NO];
}

- (void)attachToDriver:(PPDriver *)theDriv
{
	[self attachToDriver:theDriv setAsCurrentMusic:NO];
}

- (void)attachToDriver:(PPDriver *)theDriv setAsCurrentMusic:(BOOL)toSet
{
	self.attachedDriver = theDriv;
	if (toSet) {
		[attachedDriver setCurrentMusic:self];
	}
}

- (void)dealloc
{
	if (currentMusic) {
		MADDisposeMusic(&currentMusic, attachedDriver ? attachedDriver.rec : NULL);
	}
}

@end
