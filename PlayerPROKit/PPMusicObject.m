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

- (id)initWithCPath:(const char*)path library:(PPLibrary*)theLib
{
	if (self = [super init]) {
		MADLibrary *tempLib = theLib._madLib;
		char type[5];
		if (MADMusicIdentifyCString(tempLib, type, (char*)path) != noErr) {
			return nil;
		}
		if (MADLoadMusicFileCString(tempLib, &currentMusic, type, (char*)path) != noErr) {
			return nil;
		}
	}
	return self;
}

- (id)initWithURL:(NSURL *)url library:(PPLibrary *)theLib
{
	return [self initWithCPath:[[url path] fileSystemRepresentation] library:theLib];
}

- (id)initWithPath:(NSString *)url library:(PPLibrary *)theLib
{
	return [self initWithCPath:[url fileSystemRepresentation] library:theLib];
}

- (id)initWithCPath:(const char*)path driver:(PPDriver *)theDriv setAsCurrentMusic:(BOOL)toSet
{
	if (self = [self initWithCPath:path library:theDriv.theLibrary]) {
		self.attachedDriver = theDriv;
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
