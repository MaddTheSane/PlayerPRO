//
//  PPInstrumentImporterObject-private.h
//  PPMacho
//
//  Created by C.W. Betts on 9/12/14.
//
//

#import "PPPlugInCommon.h"
#import "PPInstrumentImporterObject.h"

#define kMadPlugIsSampleKey @"MADPlugIsSample"

typedef NS_OPTIONS(unsigned char, MADPlugCapabilities) {
	PPMADCanDoNothing	= 0,
	PPMADCanImport		= 1 << 0,
	PPMADCanExport		= 1 << 1,
	PPMADCanDoBoth		= PPMADCanImport | PPMADCanExport
};

@interface PPInstrumentImporterObject ()
@property (readwrite, copy) NSArray *UTITypes;
@property (readwrite, getter=isSample) BOOL sample;
@property (readwrite) BOOL canImport;
@property (readwrite) BOOL canExport;

- (instancetype)initWithBundleNoInit:(NSBundle *)tempBundle NS_DESIGNATED_INITIALIZER;

@end

static inline OSType NSStringToOSType(NSString *CFstri)
{
	return UTGetOSTypeFromString((__bridge CFStringRef)(CFstri));
}

static inline NSString* OSTypeToNSString(OSType theOSType)
{
	return CFBridgingRelease(UTCreateStringForOSType(theOSType));
}

