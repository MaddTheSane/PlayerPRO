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
- (id)initWithPlugInCPath:(char*)cPath;
@end

@implementation PPLibrary

@synthesize _madLib = theLibrary;

- (id)init
{
	return [self initWithPlugInCPath:NULL];
}

- (id)initWithPlugInCPath:(char*)cPath
{
	if (self = [super init]) {
		MADInitLibrary(cPath, NO, &theLibrary);
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
	MADDisposeLibrary(theLibrary);
	
	[super dealloc];
}

@end
