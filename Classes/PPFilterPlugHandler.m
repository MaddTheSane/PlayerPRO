//
//  PPFilterPlugHandler.m
//  PPMacho
//
//  Created by C.W. Betts on 1/22/13.
//
//

#import "PPFilterPlugHandler.h"
#import "PPFilterPlugObject.h"
#import "PPPlugInCommon.h"

@implementation PPFilterPlugHandler

@synthesize driverRec;

- (void)setDriverRec:(MADDriverRec **)aDriverRec
{
	driverRec = aDriverRec;
	theInfo.driverRec = *driverRec;
}

- (id)initWithMusic:(MADMusic**)theMus
{
	if (self = [super init]) {
		NSNotificationCenter *notCen = [NSNotificationCenter defaultCenter];
		[notCen addObserver:self selector:@selector(driverRecDidChange:) name:PPDriverDidChange object:nil];
		[notCen addObserver:self selector:@selector(madMusicDidChange:) name:PPMusicDidChange object:nil];
		curMusic = theMus;
		filterPlugs = [[NSMutableArray alloc] initWithCapacity:20];
		theInfo.RPlaySound = inMADPlaySoundData;
		theInfo.fileType = 'PLug';
		
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
					CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
					CFURLRef BundleURL = CFBundleCopyBundleURL(tempBundleRef);
					NSBundle *tempBundle = [NSBundle bundleWithURL:CFBridgingRelease(BundleURL)];
					PPFilterPlugObject *tempObj = [[PPFilterPlugObject alloc] initWithBundle:tempBundle];
					CFRelease(tempBundleRef);
					if (tempObj) {
						[filterPlugs addObject:tempObj];
						RELEASEOBJ(tempObj);
					}
				}
			}
			CFRelease(somePlugs);
		}
	}
	return self;
}

- (OSErr)callDigitalPlugAtIndex:(NSInteger)idx sampleData:(sData*)theInsData startLength:(long)start endLength:(long)end stereoMode:(short)stereo
{
	PPFilterPlugObject *tmp = [filterPlugs objectAtIndex:idx];
	return [tmp callPluginWithData:theInsData selectionStart:start selectionEnd:end plugInInfo:&theInfo stereoMode:stereo];
}

- (PPFilterPlugObject*)plugInAtIndex:(NSUInteger)idx
{
	return [filterPlugs objectAtIndex:idx];
}

- (NSUInteger)plugInCount
{
	return [filterPlugs count];
}

- (id)init
{
	[self doesNotRecognizeSelector:_cmd];
	return nil;
}

- (void)driverRecDidChange:(NSNotification*)aNot
{
	if (driverRec && *driverRec) {
		theInfo.driverRec = *driverRec;
	}
}

- (void)madMusicDidChange:(NSNotification*)aNot
{
	
}

- (void)addPlugInFromBundle:(NSBundle *)theBund
{
	PPFilterPlugObject *obj = [[PPFilterPlugObject alloc] initWithBundle:theBund];
	if (obj) {
		[filterPlugs addObject:obj];
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
	RELEASEOBJ(filterPlugs);
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	SUPERDEALLOC;
}

@end
