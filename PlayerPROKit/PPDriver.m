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

@implementation PPDriver

- (id)init
{
	NSAssert(NO, @"PPDriver cannot be inited without a library");
	return nil;
}

- (id)initWithLibrary:(PPLibrary *)theLib
{
	MADDriverSettings theSet;
	MADGetBestDriver(&theSet);
	return [self initWithLibrary:theLib settings:&theSet];
}

- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings
{
	if (self = [super init]) {
		thePPLib = [theLib retain];
		MADCreateDriver(theSettings, theLib._madLib, &theRec);
	}
	return self;
}

- (void)dealloc
{
	MADDisposeDriver(theRec);
	[thePPLib release];
	
	[super dealloc];
}

@end
