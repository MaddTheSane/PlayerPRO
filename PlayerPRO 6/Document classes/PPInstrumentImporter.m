//
//  PPInstrumentImporter.m
//  PPMacho
//
//  Created by C.W. Betts on 12/26/12.
//
//

#define ISUSINGPPPLUG 1
#import "PPInstrumentImporter.h"
#import "ARCBridge.h"
#import "PPPlugInCommon.h"
#import "PPInstrumentImporterObject.h"
#import "UserDefaultKeys.h"

@implementation PPInstrumentImporter

@synthesize driverRec;
- (void)setDriverRec:(MADDriverRec **)aTheDriver
{
	driverRec = aTheDriver;
	plugInfo.driverRec = *driverRec;
}

- (id)init
{
	[self doesNotRecognizeSelector:_cmd];
	AUTORELEASEOBJNORETURN(self);
	return nil;
}

- (NSString*)description
{
	return [NSString stringWithFormat:@"Music: %p Importers: %@", curMusic, [instrumentIEArray description]];
}

- (void)driverRecDidChange:(NSNotification *)aNot
{
	if (driverRec && *driverRec) {
		plugInfo.driverRec = *driverRec;
	} else {
		plugInfo.driverRec = NULL;
	}
}

- (void)musicDidChange:(NSNotification *)aNot
{
	
}

- (id)initWithMusic:(MADMusic**)theMus
{
	if (self = [super init]) {
		curMusic = theMus;
		plugInfo.RPlaySound = inMADPlaySoundData;
		instrumentIEArray = [[NSMutableArray alloc] initWithCapacity:10];
		
		NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
		[defaultCenter addObserver:self selector:@selector(driverRecDidChange:) name:PPDriverDidChangeNotification object:nil];
		[defaultCenter addObserver:self selector:@selector(musicDidChange:) name:PPMusicDidChangeNotification object:nil];

		NSArray *plugLocs = DefaultPlugInLocations();
		
		NSInteger x, y;
		
		for (NSURL *aPlugLoc in plugLocs) {
			CFIndex		PlugNums;
			CFArrayRef	somePlugs;
			somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, BRIDGE(CFURLRef, aPlugLoc), CFSTR("plugin"));
			PlugNums = CFArrayGetCount(somePlugs);
			if (PlugNums > 0) {
				for (x = 0; x < PlugNums; x++) {
					@autoreleasepool {
						CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
						NSBundle *tempBundle = [NSBundle bundleWithURL:CFBridgingRelease(CFBundleCopyBundleURL(tempBundleRef))];
						PPInstrumentImporterObject *tempObj = [[PPInstrumentImporterObject alloc] initWithBundle:tempBundle];
						CFRelease(tempBundleRef);
						if (tempObj) {
							for (y = 0; y < [instrumentIEArray count]; y++) {
								PPInstrumentImporterObject *toComp = instrumentIEArray[y];
								if (toComp.type == tempObj.type) {
									if (toComp.version < tempObj.version) {
										instrumentIEArray[y] = tempObj;
										tempObj = nil;
										break;
									} else {
										tempObj = nil;
										break;
									}
								}
							}
							if (tempObj) {
								[instrumentIEArray addObject:tempObj];
							}
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
	PPInstrumentImporterObject *obj = [[PPInstrumentImporterObject alloc] initWithBundle:theBund];
	NSInteger y;
	if (obj) {
		for (y = 0; y < [instrumentIEArray count]; y++) {
			PPInstrumentImporterObject *toComp = instrumentIEArray[y];
			if (toComp.type == obj.type) {
				if (toComp.version < obj.version) {
					instrumentIEArray[y] = obj;
					RELEASEOBJ(obj);
					obj = nil;
					break;
				} else {
					RELEASEOBJ(obj);
					obj = nil;
					break;
				}
			}
		}
		if (obj) {
			[instrumentIEArray addObject:obj];
		}
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

- (PPInstrumentImporterObject*)plugInAtIndex:(NSUInteger)idx
{
	return instrumentIEArray[idx];
}

- (NSInteger)plugInCount
{
	return [instrumentIEArray count];
}

- (OSErr)callInstumentPlugIn:(PPInstrumentImporterObject*)thePlug order:(OSType)theOrd instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL
{
	plugInfo.fileType = [thePlug type];
	return [thePlug importInstrument:theURL instrumentDataReference:&(*curMusic)->fid[ins] sampleDataReference:&(*curMusic)->sample[(*curMusic)->fid[ins].firstSample] instrumentSample:samp function:theOrd plugInfo:&plugInfo];
}

- (OSErr)exportInstrumentOfType:(OSType)theType instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (theType == obj.type) {
			return [self callInstumentPlugIn:obj order:MADPlugExport instrument:ins sample:samp URL:theURL];
		}
	}
	return MADCannotFindPlug;
}

- (OSErr)importInstrumentOfType:(OSType)theType instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (theType == obj.type) {
			return [self callInstumentPlugIn:obj order:MADPlugImport instrument:ins sample:samp URL:theURL];
		}
	}
	return MADCannotFindPlug;
}

- (OSErr)testInstrumentFile:(NSURL *)toTest type:(OSType)theType
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (theType == obj.type) {
			short temp;
			return [self callInstumentPlugIn:obj order:MADPlugTest instrument:0 sample:&temp URL:toTest];
		}
	}
	return MADCannotFindPlug;
}

- (OSErr)identifyInstrumentFile:(NSURL*)ref type:(OSType*)outType
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		short temp;
		if ([self callInstumentPlugIn:obj order:MADPlugTest instrument:0 sample:&temp URL:ref] == MADNoErr) {
			if (outType) {
				*outType = obj.type;
			}
			return MADNoErr;
		}
	}
	if (outType) {
		*outType = '!!!!';
	}
	return MADCannotFindPlug;
}

- (BOOL)isPlugAvailable:(OSType)kind type:(OSType*)theType
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (kind == obj.type) {
			if (theType) {
				if (obj.isSamp) {
					*theType = MADPlugSampleImporter;
				} else {
					*theType = MADPlugInstrumentImporter;
				}
			}
			return YES;
		}
	}
	if (theType) {
		*theType = MADPlugNonePlug;
	}
	return NO;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len
{
	return [instrumentIEArray countByEnumeratingWithState:state objects:buffer count:len];
}

@end
