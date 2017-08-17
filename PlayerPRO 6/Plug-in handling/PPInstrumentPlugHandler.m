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
		self.instrumentIEArray = [[NSMutableArray alloc] init];
		
		for (NSURL *aPlugLoc in plugLocs) {
			CFArrayRef	somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, (__bridge CFURLRef)aPlugLoc, CFSTR("plugin"));
			CFIndex		PlugNums = CFArrayGetCount(somePlugs);
			if (PlugNums > 0) {
				for (CFIndex x = 0; x < PlugNums; x++) {
					@autoreleasepool {
						CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
						NSBundle *tempBundle = [NSBundle bundleWithURL:CFBridgingRelease(CFBundleCopyBundleURL(tempBundleRef))];
						PPInstrumentImporterObject *tempObj = [[PPInstrumentImporterObject alloc] initWithBundle:tempBundle];
						//You can ignore the Clang static warning of incorrect decrement here.
						CFRelease(tempBundleRef);
						if (tempObj) {
							for (NSInteger y = 0; y < [instrumentIEArray count]; y++) {
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

- (MADErr)testInstrumentFile:(NSURL *)toTest type:(OSType)theType
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (theType == obj.type) {
			return [obj canImportFileAtURL:toTest] ? MADNoErr : MADFileNotSupportedByThisPlug ;
		}
	}
	return MADCannotFindPlug;
}

- (BOOL)identifyInstrumentFile:(NSURL*)ref type:(OSType*)outType error:(NSError**)error;
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if ([obj canImportFileAtURL:ref] == true) {
			if (outType) {
				*outType = obj.type;
			}
			return YES;
		}
	}
	if (outType) {
		*outType = '!!!!';
	}
	if (error) {
		*error = [NSError errorWithDomain:PPMADErrorDomain code:MADCannotFindPlug userInfo:@{NSURLErrorKey: ref}];
	}
	return NO;
}

- (void)beginExportingInstrument:(PPInstrumentObject*)theIns ofType:(OSType)aType toURL:(NSURL*)aURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handler
{
	PPInstrumentImporterObject *aPlug;
	for (PPInstrumentImporterObject *plug in instrumentIEArray) {
		if (plug.type == aType) {
			aPlug = plug;
			break;
		}
	}
	
	if (aPlug == nil) {
		handler([NSError errorWithDomain:PPMADErrorDomain code:MADCannotFindPlug userInfo:nil]);
	} else {
		[aPlug beginExportInstrument:theIns toURL:aURL driver:driver parentDocument:document handler:handler];
	}
}

- (void)beginImportingInstrumentOfType:(OSType)aType fromURL:(NSURL*)aURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(void (^)(NSError *errorCode, PPInstrumentObject *createdIns))handler
{
	PPInstrumentImporterObject *aPlug;
	for (PPInstrumentImporterObject *plug in instrumentIEArray) {
		if (plug.type == aType) {
			aPlug = plug;
			break;
		}
	}
	
	if (aPlug == nil) {
		handler([NSError errorWithDomain:PPMADErrorDomain code:MADCannotFindPlug userInfo:nil], nil);
	} else {
		[aPlug beginImportInstrumentAtURL:aURL driver:driver parentDocument:document handler:handler];
	}
}

- (BOOL)isPlugAvailable:(OSType)kind
{
	for (PPInstrumentImporterObject *obj in instrumentIEArray) {
		if (kind == obj.type) {
			return YES;
		}
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
