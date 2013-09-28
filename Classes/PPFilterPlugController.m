//
//  PPFilterPlugHandler.m
//  PPMacho
//
//  Created by C.W. Betts on 1/22/13.
//
//

#import "PPFilterPlugController.h"
#import "PPFilterPlugObject.h"
#import "PPPlugInCommon.h"
#import "UserDefaultKeys.h"

@implementation PPFilterPlugController

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
		filterPlugs = [[NSMutableArray alloc] initWithCapacity:20];
		theInfo.RPlaySound = inMADPlaySoundData;
		theInfo.fileType = 'PLug';
		
	}
	return self;
}

- (OSErr)callDigitalPlugAtIndex:(NSInteger)idx sampleData:(sData*)theInsData startLength:(long)start endLength:(long)end stereoMode:(short)stereo
{
	PPFilterPlugObject *tmp = filterPlugs[idx];
	return [tmp callPluginWithData:theInsData selectionStart:start selectionEnd:end plugInInfo:&theInfo stereoMode:stereo];
}

- (PPFilterPlugObject*)plugInAtIndex:(NSUInteger)idx
{
	return filterPlugs[idx];
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
	} else {
		theInfo.driverRec = NULL;
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
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end
