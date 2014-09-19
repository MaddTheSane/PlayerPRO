//
//  PPInstrumentImporterObject.m
//  PPMacho
//
//  Created by C.W. Betts on 12/27/12.
//
//

#import "PPPlugInCommon.h"
#import "PPInstrumentImporterObject-private.h"
#import "PPInstrumentImporterCompatObject.h"

#define PPINLoadPlug(theBundle) (PPInstrumentPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROInstrumentPlugTypeID, kPlayerPROInstrumentPlugInterfaceID)

static Class strClass;
static Class numClass;

static inline BOOL getBoolFromId(id NSType)
{
	if ([NSType isKindOfClass:numClass] || [NSType isKindOfClass:strClass]) {
		return [NSType boolValue];
	} else {
		return NO;
	}
}

@interface PPInstrumentImporterCompatObject ()
@property PPInstrumentPlugin **xxxx;
@property BOOL is32Bit;
@end

@implementation PPInstrumentImporterCompatObject
@synthesize xxxx;

- (NSString*)description
{
	NSString *typeString = OSTypeToNSString(type);
	return [NSString stringWithFormat:@"%@ - %@ Sample: %@ Type: %@ UTIs: %@", self.menuName, [self.file bundlePath], self.sample ? @"YES": @"NO", typeString, [self.UTITypes description]];
}

- (instancetype)initWithBundleNoInit:(NSBundle *)tempBundle
{
	return self = [self initWithBundle:tempBundle];
}

- (instancetype)initWithBundle:(NSBundle *)tempBundle
{
	BOOL has32 = NO, has64 = NO;
	NSArray *archs = tempBundle.executableArchitectures;
	for (NSNumber *arch in archs) {
		if ([arch isEqual:@(NSBundleExecutableArchitectureX86_64)]) {
			has64 = YES;
		} else if ([arch isEqual:@(NSBundleExecutableArchitectureI386)]) {
			has32 = YES;
		}
	}
	
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		strClass = [NSString class];
		numClass = [NSNumber class];
	});

	if (self = [super initWithBundleNoInit:tempBundle]) {
		return nil;
		NSURL *tempBundleRef = [tempBundle bundleURL];
		
		CFBundleRef tempCFBundle = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef)tempBundleRef);
		
		xxxx = PPINLoadPlug(tempCFBundle);
		
		CFRelease(tempCFBundle);
		
		if (!xxxx) {
			return nil;
		}
		
		NSMutableDictionary *tempDict = [[tempBundle infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[tempBundle localizedInfoDictionary]];
		id DictionaryTemp = [tempDict valueForKey:kMadPlugIsSampleKey];
		if ([DictionaryTemp isKindOfClass:numClass] || [DictionaryTemp isKindOfClass:strClass]) {
			self.sample = [DictionaryTemp boolValue];
		} else {
			self.sample = NO;
		}
		
		DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugTypeKey];
		if ([DictionaryTemp isKindOfClass:strClass]) {
			type = NSStringToOSType(DictionaryTemp);
		} else if ([DictionaryTemp isKindOfClass:numClass]) {
			type = [(NSNumber*)DictionaryTemp unsignedIntValue];
		} else {
			return nil;
		}
		
		{
			id canImportValue, canExportValue;
			canImportValue = [tempDict valueForKey:(__bridge NSString*)kMadPlugDoesImport];
			canExportValue = [tempDict valueForKey:(__bridge NSString*)kMadPlugDoesExport];
			if (canImportValue || canExportValue) {
				MADPlugCapabilities capabilities = PPMADCanDoNothing;
				if (canImportValue) {
					if (getBoolFromId(canImportValue))
						capabilities = PPMADCanImport;
				}
				if (canExportValue) {
					if (getBoolFromId(canExportValue))
						capabilities |= PPMADCanExport;
				}
				
				if ((capabilities & PPMADCanImport) == PPMADCanImport) {
					self.canImport = YES;
				}
				
				if ((capabilities & PPMADCanExport) == PPMADCanExport) {
					self.canExport = YES;
				}
				
				if (!self.canImport && !self.canExport) {
					return nil;
				}
				
				
			} else {
				MADPlugModes ourMode = 0;
				DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugModeKey];
				if ([DictionaryTemp isKindOfClass:strClass]) {
					ourMode = NSStringToOSType(DictionaryTemp);
				} else if ([DictionaryTemp isKindOfClass:numClass]) {
					ourMode = [(NSNumber*)DictionaryTemp unsignedIntValue];
				} else
					return nil;
				
				switch (ourMode) {
					case MADPlugImportExport:
						self.canExport = YES;
						self.canImport = YES;
						break;
						
					case MADPlugExport:
						self.canExport = YES;
						break;
						
					case MADPlugImport:
						self.canImport = YES;
						break;
						
					default:
						return nil;
						break;
				}
			}
		}
	}
	return self;
}

- (BOOL)canImportFileAtURL:(NSURL *)fileURL
{
	return NO;
}

- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
{
	return MADOrderNotImplemented;
}

- (void)dealloc
{
	if (xxxx) {
		(*xxxx)->Release(xxxx);
	}
}

- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp plugInfo:(PPInfoPlug*)plugInfo
{
	NSURL *bundleURL = [self.file bundleURL];
	CFBundleRef tempRef = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef)bundleURL);
	
	CFBundleRefNum fileID = CFBundleOpenBundleResourceMap(tempRef);
	
	OSErr returnType = (*xxxx)->InstrumentMain(xxxx, imporexp, insData, sdataref, insSamp, (__bridge CFURLRef)fileToImport, plugInfo);
	
	CFBundleCloseBundleResourceMap(tempRef, fileID);
	CFRelease(tempRef);
	
	return returnType;
}

@end
