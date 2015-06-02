//
//  PPInstrumentPlugBridgeObject.m
//  PPMacho
//
//  Created by C.W. Betts on 9/20/14.
//
//

#import "PPInstrumentPlugBridgeObject.h"
#import "PPCoreInstrumentPlugBridge.h"
#import "ARCBridge.h"
#import "PPInstrumentPlugBridgeHelper.h"

#define kMadPlugIsSampleKey @"MADPlugIsSample"

@interface PPInstrumentPlugBridgeObject ()
@property (retain, readwrite) NSBundle *bundleFile;
- (MADErr)callPlugWithOrder:(OSType)order instrument:(InstrData*)instr sampleArray:(sData**)samps sampleIndex:(short*)sampIdx URL:(NSURL*)fileURL;
@end

@implementation PPInstrumentPlugBridgeObject
@synthesize xxxx;
@synthesize bundleFile = _bundleFile;
@synthesize sample = _isSample;

- (instancetype)init
{
	return nil;
}

- (BOOL)canLoadFileAtURL:(NSURL*)theURL
{
	MADErr iErr = [self callPlugWithOrder:MADPlugTest instrument:NULL sampleArray:NULL sampleIndex:NULL URL:theURL];
	return iErr == MADNoErr;
}

- (instancetype)initWithBundle:(NSBundle *)tempBundle
{
	if (self = [super init]) {
		id sampleObj = [tempBundle infoDictionary][kMadPlugIsSampleKey];
		_isSample = [sampleObj boolValue];
		NSURL *tempBundURL = [tempBundle bundleURL];
		CFBundleRef tempCFBundle = CFBundleCreate(kCFAllocatorDefault, BRIDGE(CFURLRef, tempBundURL));
		xxxx = (PPInstrumentPlugin**)PPINLoadPlug(tempCFBundle);
		CFRelease(tempCFBundle);
		if (!xxxx) {
			return nil;
		}
		self.bundleFile = tempBundle;
	}
	return self;
}

- (MADErr)importURL:(NSURL*)fileURL instrument:(InstrData*)insData sampleArray:(sData**)sdataref sampleIndex:(short*)insSamp
{
	return [self callPlugWithOrder:MADPlugImport instrument:insData sampleArray:sdataref sampleIndex:insSamp URL:fileURL];
}

- (MADErr)callPlugWithOrder:(OSType)order instrument:(InstrData*)insData sampleArray:(sData**)sdataref sampleIndex:(short*)insSamp URL:(NSURL*)fileURL
{
	PPInfoPlug plugInfo = {0};
	NSURL *bundleURL = [self.bundleFile bundleURL];
	CFBundleRef tempRef = CFBundleCreate(kCFAllocatorDefault, BRIDGE(CFURLRef, bundleURL));
	
	CFBundleRefNum fileID = CFBundleOpenBundleResourceMap(tempRef);
	
	OSErr returnType = (*xxxx)->InstrumentMain(xxxx, order, insData, sdataref, insSamp, BRIDGE(CFURLRef, fileURL), &plugInfo);
	
	CFBundleCloseBundleResourceMap(tempRef, fileID);
	CFRelease(tempRef);
	
	return returnType;
}

- (void)dealloc
{
	if (xxxx) {
		(*xxxx)->Release(xxxx);
	}
	
#if !__has_feature(objc_arc)
	self.bundleFile = nil;
	
	[super dealloc];
#endif
}

@end
