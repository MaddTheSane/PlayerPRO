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
	return [NSString stringWithFormat:@"%@ - %@ Type: %@ UTIs: %@", self.menuName, [self.file bundlePath], typeString, [UTITypes description]];
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

		if ((!_canImport) && (!_canExport)) {
			return tryOldAPI(tempBundle);
		}
		
		self.plugCode = [[bundClass alloc] initForPlugIn];
		
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

- (MADErr)playInstrumentAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
{
	if ([plugCode conformsToProtocol:@protocol(PPInstrumentImportPlugin)] && [plugCode respondsToSelector:@selector(playInstrumentAtURL:driver:)]) {
		return [plugCode playInstrumentAtURL:aSample driver:driver];
	} else {
		return MADOrderNotImplemented;
	}
}

- (BOOL)canImportFileAtURL:(NSURL *)fileURL
{
	if ([plugCode conformsToProtocol:@protocol(PPInstrumentImportPlugin)]) {
		return [plugCode canImportInstrumentAtURL:fileURL];
	} else {
		return NO;
	}
}

- (void)beginImportInstrumentAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(void (^)(MADErr err, PPInstrumentObject* instrument))handler
{
	if (![plugCode conformsToProtocol:@protocol(PPInstrumentImportPlugin)]) {
		handler(MADOrderNotImplemented, nil);
		return;
	}

	PPInstrumentObject *outIns;
	MADErr ourErr = [plugCode importInstrumentAtURL:sampleURL instrument:&outIns driver:driver ];

	if (ourErr == MADOrderNotImplemented && [plugCode respondsToSelector:@selector(beginImportInstrumentAtURL:driver:parentWindow:handler:)]) {
		[plugCode beginImportInstrumentAtURL:sampleURL driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		handler(ourErr, outIns);
	}

}

- (void)beginExportInstrument:(PPInstrumentObject*)anIns toURL:(NSURL*)sampURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handler
{
	if (![plugCode conformsToProtocol:@protocol(PPInstrumentExportPlugin)]) {
		handler(MADOrderNotImplemented);
		return;
	}
	
	MADErr ourErr = [plugCode exportInstrument:anIns toURL:sampURL driver:driver];
	if (ourErr == MADOrderNotImplemented && [plugCode respondsToSelector:@selector(beginExportInstrument:toURL:driver:parentWindow:handler:)]) {
		[plugCode beginExportInstrument:anIns toURL:sampURL driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		handler(ourErr);
	}

}

@end
