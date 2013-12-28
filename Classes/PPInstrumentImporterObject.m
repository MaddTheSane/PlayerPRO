//
//  PPInstrumentImporterObject.m
//  PPMacho
//
//  Created by C.W. Betts on 12/27/12.
//
//

#define ISUSINGPPPLUG 1
#import "PPInstrumentImporterObject.h"
#import "PPPlugInCommon.h"

NSString * const kMadPlugIsSampleKey = @"MADPlugIsSample";

#define PPINLoadPlug(theBundle) (PPInstrumentPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROInstrumentPlugTypeID, kPlayerPROInstrumentPlugInterfaceID)

static inline OSType NSStringToOSType(NSString *CFstri)
{
	const char *thecOSType = [CFstri cStringUsingEncoding:NSMacOSRomanStringEncoding];
	
	return Ptr2OSType(thecOSType);
}

static inline BOOL getBoolFromId(id NSType)
{
	if ([NSType isKindOfClass:[NSNumber class]] || [NSType isKindOfClass:[NSString class]]) {
		return [NSType boolValue];
	} else {
		return NO;
	}
}

@interface PPInstrumentImporterObject ()
@property (readwrite, copy) NSArray *UTITypes;
@property (readwrite) OSType mode;
@property (readwrite) BOOL isSample;
@end

@implementation PPInstrumentImporterObject
@synthesize UTITypes;
@synthesize mode;
@synthesize isSample = isSamp;

typedef enum _MADPlugCapabilities {
	PPMADCanDoNothing	= 0,
	PPMADCanImport		= 1 << 0,
	PPMADCanExport		= 1 << 1,
	PPMADCanDoBoth		= PPMADCanImport | PPMADCanExport
} MADPlugCapabilities;

- (NSString*)description
{
	char typeString[5] = {0};
	OSType2Ptr(type, typeString);
	return [NSString stringWithFormat:@"%@ - %@ Sample: %@ Type: %@ UTIs: %@", self.menuName, [self.file bundlePath], isSamp ? @"YES": @"NO", [NSString stringWithCString:typeString encoding:NSMacOSRomanStringEncoding], [UTITypes description]];
}

static Class strClass;
static Class numClass;

- (instancetype)initWithBundle:(NSBundle *)tempBundle
{
	if (self = [super initWithBundle:tempBundle]) {
		static dispatch_once_t onceToken;
		dispatch_once(&onceToken, ^{
			strClass = [NSString class];
			numClass = [NSNumber class];
		});
		{
			NSURL *tempBundleRef = [tempBundle bundleURL];
			
			CFBundleRef tempCFBundle = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef)tempBundleRef);
			
			xxxx = PPINLoadPlug(tempCFBundle);
			
			CFRelease(tempCFBundle);

			if (!xxxx) {
				return nil;
			}
		}
		
		NSMutableDictionary *tempDict = [[tempBundle infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[tempBundle localizedInfoDictionary]];
		id DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugUTITypesKey];
		if ([DictionaryTemp isKindOfClass:[NSArray class]]) {
			self.UTITypes = [DictionaryTemp copy];
		} else if ([DictionaryTemp isKindOfClass:strClass]) {
			self.UTITypes = @[[NSString stringWithString:DictionaryTemp]];
		} else
			return nil;
		
		
		DictionaryTemp = [tempDict valueForKey:kMadPlugIsSampleKey];
		if ([DictionaryTemp isKindOfClass:numClass] || [DictionaryTemp isKindOfClass:strClass]) {
			self.isSample = [DictionaryTemp boolValue];
		} else {
			self.isSample = NO;
		}
		
		DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugTypeKey];
		if ([DictionaryTemp isKindOfClass:strClass]) {
			type = NSStringToOSType(DictionaryTemp);
		} else if([DictionaryTemp isKindOfClass:numClass]) {
			type = [(NSNumber*)DictionaryTemp unsignedIntValue];
		} else
			return nil;
		
		{
			id canImportValue = nil, canExportValue = nil;
			canImportValue = [tempDict valueForKey:(__bridge NSString*)kMadPlugDoesImport];
			canExportValue = [tempDict valueForKey:(__bridge NSString*)kMadPlugDoesExport];
			if (canImportValue || canExportValue) {
				MADPlugCapabilities capabilities = PPMADCanDoNothing;
				if (canImportValue) {
					if(getBoolFromId(canImportValue))
						capabilities = PPMADCanImport;
				}
				if (canExportValue) {
					if(getBoolFromId(canExportValue))
						capabilities |= PPMADCanExport;
				}
				
				switch (capabilities) {
					case PPMADCanImport:
						self.mode = MADPlugImport;
						break;
						
					case PPMADCanExport:
						self.mode = MADPlugExport;
						break;
						
					case PPMADCanDoBoth:
						self.mode = MADPlugImportExport;
						break;
						
					default:
						return nil;
						break;
				}
			} else {
				DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugModeKey];
				if ([DictionaryTemp isKindOfClass:strClass]) {
					self.mode = NSStringToOSType(DictionaryTemp);
				} else if([DictionaryTemp isKindOfClass:numClass]) {
					self.mode = [(NSNumber*)DictionaryTemp unsignedIntValue];
				} else
					return nil;
			}
		}
	}
	return self;
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
	
	OSErr returnType = (*xxxx)->InstrMain(xxxx, imporexp, insData, sdataref, insSamp, (__bridge CFURLRef)fileToImport, plugInfo);
	
	CFBundleCloseBundleResourceMap(tempRef, fileID);
	CFRelease(tempRef);
	
	return returnType;
}

- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp
{
	return [self importInstrument:fileToImport instrumentDataReference:insData sampleDataReference:sdataref instrumentSample:insSamp function:imporexp plugInfo:NULL];
}

@end
