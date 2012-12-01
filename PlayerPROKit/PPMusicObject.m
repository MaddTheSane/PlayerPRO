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

@synthesize _currentMusic = currentMusic;

- (id)initWithCPath:(const char*)path driver:(PPDriver *)theDriv setAsCurrentMusic:(BOOL)toSet
{
	if (self = [super init]) {
		MADLibrary *tempLib = theDriv.theLibrary._madLib;
		char type[5];
		attachedDriver = [theDriv retain];
		if (MADMusicIdentifyCString(tempLib, type, (char*)path) != noErr) {
			[self release];
			return nil;
		}
		//MADMusicIdentifyCString(tempLib, type, path);
		if (MADLoadMusicFileCString(tempLib, &currentMusic, type, (char*)path) != noErr) {
			[self release];
			return nil;
		}
		if (toSet) {
			[attachedDriver setCurrentMusic:self];
		}
	}
	return self;
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
	return [self initWithCPath:[[url path] fileSystemRepresentation] driver:theLib setAsCurrentMusic:toSet];
}

- (id)initWithPath:(NSString *)url driver:(PPDriver *)theLib setAsCurrentMusic:(BOOL)toSet
{
	return [self initWithCPath:[url fileSystemRepresentation] driver:theLib setAsCurrentMusic:toSet];
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
	if (attachedDriver) {
		[attachedDriver release];
	}
	attachedDriver = [theDriv retain];
}

- (void)dealloc
{
	if (!attachedDriver) {
		NSAssert(attachedDriver, @"Music released without driver!");
		if (currentMusic) {
			MADDisposeMusic(&currentMusic, NULL);
		}
	}else{
		if (currentMusic) {
			MADDisposeMusic(&currentMusic, attachedDriver.rec);
		}
		[attachedDriver release];
	}
	
	[super dealloc];
}

@end
