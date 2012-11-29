//
//  PPLibrary.m
//  PPMacho
//
//  Created by C.W. Betts on 10/28/12.
//
//

#import "PPLibrary.h"
#import "PPLibrary_PPKPrivate.h"

@interface PPLibrary ()
- (id)initWithPlugInCPath:(const char*)cPath;
@end

@implementation PPLibrary

@synthesize _madLib = theLibrary;

- (id)init
{
	return [self initWithPlugInCPath:NULL];
}

- (id)initWithPlugInCPath:(const char*)cPath
{
	if (self = [super init]) {
		if(MADInitLibrary((char*)cPath, &theLibrary) != noErr)
		{
			[self release];
			return nil;
		}
	}
	return self;
}

- (id)initWithPlugInPath:(NSString *)path
{
	return [self initWithPlugInCPath:[path fileSystemRepresentation]];
}

- (id)initWithPlugInURL:(NSURL *)URL
{
	return [self initWithPlugInCPath:[[URL path] fileSystemRepresentation]];
}

- (void)dealloc
{
	if(theLibrary)
		MADDisposeLibrary(theLibrary);
	
	[super dealloc];
}

@end
