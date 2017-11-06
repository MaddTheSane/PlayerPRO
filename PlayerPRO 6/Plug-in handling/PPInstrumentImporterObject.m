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

static PPInstrumentImporterCompatObject *tryOldAPI(NSBundle *theBundle) NS_RETURNS_RETAINED;
PPInstrumentImporterCompatObject *tryOldAPI(NSBundle *theBundle)
{
	NSDictionary *bundDict = theBundle.infoDictionary;
	NSDictionary *plugInTypes = bundDict[@"CFPlugInTypes"];
	if (!plugInTypes) {
		return nil;
	}
	
	for (NSString *key in plugInTypes) {
		NSString *insString = CFBridgingRelease(CFUUIDCreateString(kCFAllocatorDefault, kPlayerPROInstrumentPlugTypeID));
		NSUUID *insUUID = [[NSUUID alloc] initWithUUIDString:insString];
		NSUUID *keyUUID = [[NSUUID alloc] initWithUUIDString:key];
		
		if ([keyUUID isEqual:insUUID]) {
			return [[PPInstrumentImporterCompatObject alloc] initWithBundle:theBundle];
		}
	}
	
	return nil;
}

@interface PPInstrumentImporterObject ()
@property (strong) id<PPInstrumentImportPlugin, PPInstrumentExportPlugin> plugCode;
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
		NSDictionary *tempDict = [tempBundle infoDictionary];
		id DictionaryTemp;
		
		DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugUTITypesKey];
		if ([DictionaryTemp isKindOfClass:[NSArray class]]) {
			self.UTITypes = DictionaryTemp;
		} else if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			self.UTITypes = @[[NSString stringWithString:DictionaryTemp]];
		} else {
			return nil;
		}
		
		DictionaryTemp = [tempDict valueForKey:(__bridge NSString*)kMadPlugTypeKey];
		if ([DictionaryTemp isKindOfClass:[NSString class]]) {
			type = NSStringToOSType(DictionaryTemp);
		} else if ([DictionaryTemp isKindOfClass:[NSNumber class]]) {
			type = [(NSNumber*)DictionaryTemp unsignedIntValue];
		} else {
			return nil;
		}
	}
	return self;
}

- (instancetype)initWithBundle:(NSBundle *)tempBundle
{
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
		if (bundClass == nil) {
			return tryOldAPI(tempBundle);
		}
		
		if ([bundClass conformsToProtocol:@protocol(PPInstrumentExportPlugin)]) {
			self.canExport = YES;
		}
		
		if ([bundClass conformsToProtocol:@protocol(PPInstrumentImportPlugin)]) {
			self.canImport = YES;
		}

		if (!(_canImport || _canExport)) {
			// exclude plug-ins that can be initialized with a different class
			if ([bundClass conformsToProtocol:@protocol(PPSampleImportPlugin)]) {
				return nil;
			} else if ([bundClass conformsToProtocol:@protocol(PPSampleExportPlugin)]) {
				return nil;
			} else if ([bundClass conformsToProtocol:@protocol(PPDigitalPlugin)]) {
				return nil;
			} else if ([bundClass conformsToProtocol:@protocol(PPFilterPlugin)]) {
				return nil;
			}

			return tryOldAPI(tempBundle);
		}
		
		self.plugCode = [[bundClass alloc] initForPlugIn];
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

- (void)beginImportInstrumentAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(void (^)(NSError *err, PPInstrumentObject* instrument))handler
{
	if (![plugCode conformsToProtocol:@protocol(PPInstrumentImportPlugin)]) {
		handler([NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:nil], nil);
		return;
	}

	PPInstrumentObject *outIns;
	NSError *ourErr;
	BOOL success = [plugCode importInstrumentAtURL:sampleURL instrument:&outIns driver:driver error:&ourErr];

	if (!success && isOrderNotImplemented(ourErr) && [plugCode respondsToSelector:@selector(beginImportInstrumentAtURL:driver:parentWindow:handler:)]) {
		[plugCode beginImportInstrumentAtURL:sampleURL driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		handler(success ? nil : ourErr, outIns);
	}

}

- (void)beginExportInstrument:(PPInstrumentObject*)anIns toURL:(NSURL*)sampURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handler
{
	if (![plugCode conformsToProtocol:@protocol(PPInstrumentExportPlugin)]) {
		handler([NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:nil]);
		return;
	}
	
	NSError *ourErr;
	BOOL success = [plugCode exportInstrument:anIns toURL:sampURL driver:driver error:&ourErr];
	if (!success && isOrderNotImplemented(ourErr) && [plugCode respondsToSelector:@selector(beginExportInstrument:toURL:driver:parentWindow:handler:)]) {
		[plugCode beginExportInstrument:anIns toURL:sampURL driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		handler(success ? nil : ourErr);
	}
}

@end
