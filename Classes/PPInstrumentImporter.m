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

@implementation PPInstrumentImporter

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

- (id)initWithMusic:(MADMusic**)theMus
{
	if (self = [super init]) {
		curMusic = theMus;
		instrumentIEArray = [[NSMutableArray alloc] initWithCapacity:20];

		NSArray *plugLocs = DefaultPlugInLocations();
		
		NSInteger PlugLocNums = [plugLocs count], i, x, y;
		
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
						PPInstrumentImporterObject *tempObj = [[PPInstrumentImporterObject alloc] initWithBundle:tempBundle];
						CFRelease(tempBundleRef);
						if (tempObj) {
							for (y = 0; y < [instrumentIEArray count]; y++) {
								PPInstrumentImporterObject *toComp = [instrumentIEArray objectAtIndex:y];
								if (toComp.type == tempObj.type) {
									if (toComp.version < tempObj.version) {
										[instrumentIEArray replaceObjectAtIndex:y withObject:tempObj];
										RELEASEOBJ(tempObj);
										tempObj = nil;
										break;
									} else {
										RELEASEOBJ(tempObj);
										tempObj = nil;
										break;
									}
								}
							}
							if (tempObj) {
								[instrumentIEArray addObject:tempObj];
								RELEASEOBJ(tempObj);
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
			PPInstrumentImporterObject *toComp = [instrumentIEArray objectAtIndex:y];
			if (toComp.type == obj.type) {
				if (toComp.version < obj.version) {
					[instrumentIEArray replaceObjectAtIndex:y withObject:obj];
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
			RELEASEOBJ(obj);
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
	return [instrumentIEArray objectAtIndex:idx];
}

- (NSInteger)plugInCount
{
	return [instrumentIEArray count];
}

- (OSErr)callInstumentPlugIn:(PPInstrumentImporterObject*)thePlug order:(OSType)theOrd instrument:(short)ins sample:(short*)samp URL:(NSURL*)theURL
{
	return [thePlug importInstrument:theURL instrumentDataReference:&(*curMusic)->fid[ ins] sampleDataReference:&(*curMusic)->sample[ (*curMusic)->fid[ ins].firstSample] instrumentSample:samp function:theOrd];
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
		if ([self callInstumentPlugIn:obj order:MADPlugTest instrument:0 sample:&temp URL:ref] == noErr) {
			if (outType) {
				*outType = obj.type;
			}
			return noErr;
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

#if !__has_feature(objc_arc)
- (void)dealloc
{
	[instrumentIEArray release];
	
	[super dealloc];
}
#endif

@end
