//
//  PPInstrumentImporterObject.m
//  PPMacho
//
//  Created by C.W. Betts on 12/27/12.
//
//

#import "PPPlugInCommon.h"
#import "PPInstrumentImporterObject.h"
#import "PPInstrumentImporterObject-private.h"
#import "PPInstrumentImporterCompatObject.h"
#import "PlayerPRO_6-Swift.h"

static Class strClass;
static Class numClass;

static PPInstrumentImporterCompatObject *tryOldAPI(NSBundle *theBundle) NS_RETURNS_RETAINED;
PPInstrumentImporterCompatObject *tryOldAPI(NSBundle *theBundle)
{
	return [[PPInstrumentImporterCompatObject alloc] initWithBundle:theBundle];
}

@interface PPInstrumentImporterObject ()
@property (strong) id<PPInstrumentImportPlugin, PPInstrumentExportPlugin> plugCode;
@property BOOL hasUI;
@end

@implementation PPInstrumentImporterObject
@synthesize UTITypes;
@synthesize sample = isSamp;
@synthesize plugCode;

- (MADPlugModes)mode
{
	MADPlugCapabilities cap = PPMADCanDoNothing;
	if (self.canImport) {
		cap = PPMADCanImport;
	}
	
	if (self.canExport) {
		cap |= PPMADCanExport;
	}
	MADPlugModes toRet = MADPlugNonePlug;
	
	switch (cap) {
		case PPMADCanImport:
			toRet = MADPlugImport;
			break;
			
		case PPMADCanExport:
			toRet = MADPlugExport;
			break;
			
		case PPMADCanDoBoth:
			toRet = MADPlugImportExport;
			break;
			
		default:
			break;
	}
	
	return toRet;
}

- (NSString*)description
{
	NSString *typeString = OSTypeToNSString(type);
	return [NSString stringWithFormat:@"%@ - %@ Sample: %@ Type: %@ UTIs: %@", self.menuName, [self.file bundlePath], isSamp ? @"YES": @"NO", typeString, [UTITypes description]];
}

- (instancetype)initWithBundleNoInit:(NSBundle *)tempBundle
{
	if (self = [super initWithBundle:tempBundle]) {
		NSMutableDictionary *tempDict = [[tempBundle infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[tempBundle localizedInfoDictionary]];
		id DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugUTITypesKey];
		if ([DictionaryTemp isKindOfClass:[NSArray class]]) {
			self.UTITypes = [DictionaryTemp copy];
		} else if ([DictionaryTemp isKindOfClass:strClass]) {
			self.UTITypes = @[[NSString stringWithString:DictionaryTemp]];
		} else {
			return nil;
		}
	}
	return self;
}

- (instancetype)initWithBundle:(NSBundle *)tempBundle
{
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		strClass = [NSString class];
		numClass = [NSNumber class];
	});

	if (self = [self initWithBundleNoInit:tempBundle]) {
		{
			NSArray *archs = tempBundle.executableArchitectures;
			BOOL hasArch = NO;
			for (NSNumber *arch in archs) {
				if ([arch isEqual:@(NSBundleExecutableArchitectureX86_64)]) {
					hasArch = YES;
					break;
				}
			}
			
			if (!hasArch) {
				return tryOldAPI(tempBundle);
			}
		}
		
		Class bundClass = [tempBundle principalClass];
		if ([bundClass conformsToProtocol:@protocol(PPInstrumentExportPlugin)]) {
			self.canExport = YES;
		}
		
		if ([bundClass conformsToProtocol:@protocol(PPInstrumentImportPlugin)]) {
			self.canImport = YES;
		}

		if ((!self.canImport) && (!self.canExport)) {
			return tryOldAPI(tempBundle);
		}
		
		self.plugCode = [[bundClass alloc] initForPlugIn];
		self.sample = ![self.plugCode isInstrument];
		self.hasUI = [self.plugCode hasUIConfiguration];
		
		NSMutableDictionary *tempDict = [[tempBundle infoDictionary] mutableCopy];
		[tempDict addEntriesFromDictionary:[tempBundle localizedInfoDictionary]];
		id DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugTypeKey];
		if ([DictionaryTemp isKindOfClass:strClass]) {
			type = NSStringToOSType(DictionaryTemp);
		} else if ([DictionaryTemp isKindOfClass:numClass]) {
			type = [(NSNumber*)DictionaryTemp unsignedIntValue];
		} else {
			return nil;
		}
	}
	return self;
}

- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
{
	if ([plugCode conformsToProtocol:@protocol(PPInstrumentImportPlugin)] && [plugCode respondsToSelector:@selector(playSampleAtURL:driver:)]) {
		return [plugCode playSampleAtURL:aSample driver:driver];
	} else {
		return MADOrderNotImplemented;
	}
}

- (BOOL)canImportFileAtURL:(NSURL *)fileURL
{
	if ([plugCode conformsToProtocol:@protocol(PPInstrumentImportPlugin)]) {
		return [plugCode canImportSampleAtURL:fileURL];
	} else {
		return NO;
	}
}

- (void)beginImportSampleAtURL:(NSURL*)sampleURL instrument:(inout PPInstrumentObject*)InsHeader sample:(inout PPSampleObject*)sample sampleID:(inout short*)sampleID driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handler
{
	if (![plugCode conformsToProtocol:@protocol(PPInstrumentImportPlugin)]) {
		handler(MADOrderNotImplemented);
		return;
	}
	
	if (self.hasUI) {
		[plugCode beginImportSampleAtURL:sampleURL instrument:InsHeader sample:sample sampleID:sampleID driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		MADErr ourErr = [plugCode importSampleAtURL:sampleURL instrument:InsHeader sample:sample sampleID:sampleID driver:driver];
		handler(ourErr);
	}
}

- (void)beginExportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handler
{
	if (![plugCode conformsToProtocol:@protocol(PPInstrumentExportPlugin)]) {
		handler(MADOrderNotImplemented);
		return;
	}
	
	if (self.hasUI) {
		[plugCode beginExportSampleToURL:sampleURL instrument:InsHeader sample:sample sampleID:sampleID driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		MADErr ourErr = [plugCode exportSampleToURL:sampleURL instrument:InsHeader sample:sample sampleID:sampleID driver:driver];
		handler(ourErr);
	}

}

@end
