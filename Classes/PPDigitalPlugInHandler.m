//
//  PPDigitalPlugInHandler.m
//  PPMacho
//
//  Created by C.W. Betts on 1/1/13.
//
//

#import "PPDigitalPlugInHandler.h"
#import "PPDigitalPlugInObject.h"
#import "PPPlugInCommon.h"
//#import <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/RDriverInt.h>

@implementation PPDigitalPlugInHandler

@synthesize driverRec;

- (void)setDriverRec:(MADDriverRec **)aDriverRec
{
	driverRec = aDriverRec;
	theInfo.driverRec = *driverRec;
}

- (OSErr)callDigitalPlugIn:(NSUInteger)plugNum pcmd:(Pcmd*)myPcmd
{
	PPDigitalPlugInObject *tmp = [digitalPlugs objectAtIndex:plugNum];
	return [tmp callWithPcmd:myPcmd plugInfo:&theInfo];
}

- (PPDigitalPlugInObject*)plugInAtIndex:(NSUInteger)idx
{
	return [digitalPlugs objectAtIndex:idx];
}

- (NSUInteger)plugInCount
{
	return [digitalPlugs count];
}

- (id)init
{
	[self doesNotRecognizeSelector:_cmd];
	AUTORELEASEOBJNORETURN(self);
	return nil;
}

- (void)driverRecDidChange:(NSNotification*)aNot
{
	if (driverRec && *driverRec) {
		theInfo.driverRec = *driverRec;
	} else {
		theInfo.driverRec = NULL;
	}
}

- (void)madMusicDidChange:(NSNotification*)aNot
{
	
}

- (id)initWithMusic:(MADMusic**)theMus
{
	if (self = [super init]) {
		NSNotificationCenter *notCen = [NSNotificationCenter defaultCenter];
		[notCen addObserver:self selector:@selector(driverRecDidChange:) name:PPDriverDidChange object:nil];
		[notCen addObserver:self selector:@selector(madMusicDidChange:) name:PPMusicDidChange object:nil];
		curMusic = theMus;
		digitalPlugs = [[NSMutableArray alloc] initWithCapacity:20];
		theInfo.RPlaySound = inMADPlaySoundData;
		theInfo.fileType = 'PPDG';

		NSArray *plugLocs = DefaultPlugInLocations();
		
		NSInteger PlugLocNums = [plugLocs count], i, x;
		
		for (i = 0; i < PlugLocNums; i++) {
			CFIndex		PlugNums;
			CFArrayRef	somePlugs;
			NSURL		*aPlugLoc;
			aPlugLoc = [plugLocs objectAtIndex:i];
			somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, BRIDGE(CFURLRef, aPlugLoc), NULL);
			PlugNums = CFArrayGetCount( somePlugs );
			if (PlugNums > 0) {
				for (x = 0; x < PlugNums; x++) {
					@autoreleasepool {
						CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
						CFURLRef BundleURL = CFBundleCopyBundleURL(tempBundleRef);
						NSBundle *tempBundle = [NSBundle bundleWithURL:CFBridgingRelease(BundleURL)];
						PPDigitalPlugInObject *tempObj = [[PPDigitalPlugInObject alloc] initWithBundle:tempBundle];
						CFRelease(tempBundleRef);
						if (tempObj) {
							[digitalPlugs addObject:tempObj];
							RELEASEOBJ(tempObj);
						}
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
		RELEASEOBJ(obj);
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

- (void)dealloc
{
	RELEASEOBJ(digitalPlugs);
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	SUPERDEALLOC;
}

@end
