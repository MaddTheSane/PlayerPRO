//
//  PPInstrumentPlugHandler.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/13.
//
//

#import "PPInstrumentPlugHandler.h"
#import "PPInstrumentImporterObject.h"
#import "PPPlugInCommon.h"

@interface PPInstrumentPlugHandler ()
@property (strong) NSMutableArray *instrumentIEArray;
@end

@implementation PPInstrumentPlugHandler
@synthesize instrumentIEArray;

- (NSArray*)plugInArray
{
	return [instrumentIEArray copy];
}

- (instancetype)init
{
	if (self = [super init]) {
		NSArray *plugLocs = DefaultPlugInLocations();
		
		NSInteger x, y;
		
		for (NSURL *aPlugLoc in plugLocs) {
			CFIndex		PlugNums;
			CFArrayRef	somePlugs;
			somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, (__bridge CFURLRef)aPlugLoc, CFSTR("plugin"));
			PlugNums = CFArrayGetCount(somePlugs);
			if (PlugNums > 0) {
				for (x = 0; x < PlugNums; x++) {
					@autoreleasepool {
						CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
						NSBundle *tempBundle = [NSBundle bundleWithURL:CFBridgingRelease(CFBundleCopyBundleURL(tempBundleRef))];
						PPInstrumentImporterObject *tempObj = [[PPInstrumentImporterObject alloc] initWithBundle:tempBundle];
						//You can ignore the Clang static warning of incorrect decrement here.
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
					obj = nil;
					break;
				} else {
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

- (PPInstrumentImporterObject *)objectAtIndexedSubscript:(NSInteger)index
{
	return [self plugInAtIndex:index];
}

- (NSInteger)plugInCount
{
	return [instrumentIEArray count];
}

- (OSErr)callInstumentPlugIn:(PPInstrumentImporterObject*)thePlug order:(OSType)theOrd instrumentReference:(InstrData*)ins sampleReference:(sData**)sampRef sample:(short*)samp URL:(NSURL*)theURL plugInfo:(PPInfoPlug *)plugInfo
{
	plugInfo->fileType = [thePlug type];
	return [thePlug importInstrument:theURL instrumentDataReference:ins sampleDataReference:sampRef instrumentSample:samp function:theOrd plugInfo:plugInfo];
}

- (OSErr)exportInstrumentOfType:(OSType)theType instrumentReference:(InstrData*)ins sampleReference:(sData**)sampRef sample:(short*)samp URL:(NSURL*)theURL plugInfo:(PPInfoPlug *)plugInfo
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (theType == obj.type) {
			return [self callInstumentPlugIn:obj order:MADPlugExport instrumentReference:ins sampleReference:sampRef sample:samp URL:theURL plugInfo:plugInfo];
		}
	}
	return MADCannotFindPlug;
}

- (OSErr)importInstrumentOfType:(OSType)theType instrumentReference:(InstrData*)ins sampleReference:(sData**)sampRef sample:(short*)samp URL:(NSURL*)theURL plugInfo:(PPInfoPlug *)plugInfo
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (theType == obj.type) {
			return [self callInstumentPlugIn:obj order:MADPlugImport instrumentReference:ins sampleReference:sampRef sample:samp URL:theURL plugInfo:plugInfo];
		}
	}
	return MADCannotFindPlug;
}

- (OSErr)testInstrumentFile:(NSURL *)toTest type:(OSType)theType
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (theType == obj.type) {
			short temp;
			PPInfoPlug tempPlug;
			return [self callInstumentPlugIn:obj order:MADPlugTest instrumentReference:NULL sampleReference:NULL sample:&temp URL:toTest plugInfo:&tempPlug];
		}
	}
	return MADCannotFindPlug;
}

- (OSErr)identifyInstrumentFile:(NSURL*)ref type:(OSType*)outType
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		short temp;
		PPInfoPlug tempPlug;
		if ([self callInstumentPlugIn:obj order:MADPlugTest instrumentReference:NULL sampleReference:NULL sample:&temp URL:ref plugInfo:&tempPlug] == MADNoErr) {
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
				if (obj.isSample) {
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
