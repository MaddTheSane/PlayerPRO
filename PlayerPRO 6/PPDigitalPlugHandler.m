//
//  PPDigitalPlugInHandler.m
//  PPMacho
//
//  Created by C.W. Betts on 1/1/13.
//
//

#import "PPDigitalPlugHandler.h"
#import "PPDigitalPlugInObject.h"
#import "PPPlugInCommon.h"
#include <PlayerPROCore/RDriverInt.h>
#import "UserDefaultKeys.h"

@interface PPDigitalPlugHandler ()
@property (strong) NSMutableArray *digitalPlugs;
@end

@implementation PPDigitalPlugHandler
@synthesize digitalPlugs;

- (OSErr)callDigitalPlugIn:(NSUInteger)plugNum pcmd:(Pcmd*)myPcmd plugInfo:(PPInfoPlug *)theInfo
{
	theInfo->fileType = 'PPDG';
	PPDigitalPlugInObject *tmp = digitalPlugs[plugNum];
	return [tmp callWithPcmd:myPcmd plugInfo:theInfo];
}

- (PPDigitalPlugInObject*)plugInAtIndex:(NSUInteger)idx
{
	return digitalPlugs[idx];
}

- (NSUInteger)plugInCount
{
	return [digitalPlugs count];
}

- (id)init
{
	if (self = [super init]) {
		self.digitalPlugs = [[NSMutableArray alloc] initWithCapacity:20];
		
		NSArray *plugLocs = DefaultPlugInLocations();
		
		NSInteger x;
		
		for (NSURL *aPlugLoc in plugLocs) {
			CFIndex		PlugNums;
			CFArrayRef	somePlugs;
			somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, (__bridge CFURLRef) aPlugLoc, CFSTR("plugin"));
			PlugNums = CFArrayGetCount(somePlugs);
			for (x = 0; x < PlugNums; x++) {
				@autoreleasepool {
					CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
					NSBundle *tempBundle = [NSBundle bundleWithURL:CFBridgingRelease(CFBundleCopyBundleURL(tempBundleRef))];
					PPDigitalPlugInObject *tempObj = [[PPDigitalPlugInObject alloc] initWithBundle:tempBundle];
					//You can ignore the Clang static warning of incorrect decrement here.
					CFRelease(tempBundleRef);
					if (tempObj) {
						[digitalPlugs addObject:tempObj];
					}
				}
			}
			CFRelease(somePlugs);
		}
	}
	return self;
}

- (void)addPlugInFromBundle:(NSBundle *)theBund
{
	PPDigitalPlugInObject *obj = [[PPDigitalPlugInObject alloc] initWithBundle:theBund];
	if (obj) {
		[digitalPlugs addObject:obj];
	}
}

- (void)addPlugInFromURL:(NSURL *)urlpath
{
	NSBundle *theBund = [NSBundle bundleWithURL:urlpath];
	[self addPlugInFromBundle:theBund];
}

- (void)addPlugInFromPath:(NSString*)thePath
{
	NSBundle *theBund = [NSBundle bundleWithPath:thePath];
	[self addPlugInFromBundle:theBund];
}

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len
{
	return [digitalPlugs countByEnumeratingWithState:state objects:buffer count:len];
}

@end
