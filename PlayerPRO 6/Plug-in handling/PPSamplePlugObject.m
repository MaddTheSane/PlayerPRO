//
//  PPSamplePlugObject.m
//  PPMacho
//
//  Created by C.W. Betts on 2/10/15.
//
//

#import "PPSamplePlugObject.h"
#import "PPSamplePlugObject-private.h"
#import "PlayerPRO_6-Swift.h"
#import <PlayerPROKit/PlayerPROKit.h>

#import "PPSamplePlugCompatObject.h"

@interface PPSamplePlugObject ()
@property (strong) id<PPSampleImportPlugin, PPSampleExportPlugin> plugCode;
@end

static PPSamplePlugCompatObject *tryOldAPI(NSBundle *theBundle) NS_RETURNS_RETAINED;
PPSamplePlugCompatObject *tryOldAPI(NSBundle *theBundle)
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
			return [[PPSamplePlugCompatObject alloc] initWithBundle:theBundle];
		}
	}
	
	return nil;
}


@implementation PPSamplePlugObject

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

- (MADPlugModes)mode
{
	MADPlugCapabilities cap = PPMADCanDoNothing;
	if (self.canImport) {
		cap = PPMADCanImport;
	}
	
	if (self.canExport) {
		cap |= PPMADCanExport;
	}
	MADPlugModes toRet;
	
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
			toRet = MADPlugNonePlug;
			break;
	}
	
	return toRet;
}

- (NSString*)description
{
	NSString *typeString = OSTypeToNSString(type);
	return [NSString stringWithFormat:@"%@ - %@ Type: %@ UTIs: %@", self.menuName, [self.file bundlePath], typeString, [_UTITypes description]];
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
		
		if ([bundClass conformsToProtocol:@protocol(PPSampleExportPlugin)]) {
			self.canExport = YES;
		}
		
		if ([bundClass conformsToProtocol:@protocol(PPSampleImportPlugin)]) {
			self.canImport = YES;
		}
		
		if ((!_canImport) && (!_canExport)) {
			// exclude plug-ins that can be detected as a different class
			if ([bundClass conformsToProtocol:@protocol(PPInstrumentImportPlugin)]) {
				return nil;
			} else if ([bundClass conformsToProtocol:@protocol(PPInstrumentExportPlugin)]) {
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


- (BOOL)canImportFileAtURL:(NSURL *)fileURL
{
	if ([_plugCode conformsToProtocol:@protocol(PPSampleImportPlugin)]) {
		return [_plugCode canImportSampleAtURL:fileURL];
	}
	return NO;
}

- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver
{
	if ([_plugCode respondsToSelector:@selector(playSampleAtURL:driver:)]) {
		return [_plugCode playSampleAtURL:aSample driver: driver];
	}
	return MADOrderNotImplemented;
}

- (void)beginImportSampleAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(void (^)(MADErr theErr, PPSampleObject *aSample))handler
{
	if (![_plugCode conformsToProtocol:@protocol(PPSampleImportPlugin)]) {
		handler(MADOrderNotImplemented, nil);
		return;
	}
	
	if ([_plugCode hasUIForImport]) {
		PPSampleObject *aSampObj;
		MADErr iErr = [_plugCode importSampleAtURL:sampleURL sample:&aSampObj driver:driver];
		if (iErr != MADOrderNotImplemented) {
			handler(iErr, aSampObj);
			return;
		}
	}
	if ([_plugCode respondsToSelector:@selector(beginImportSampleAtURL:driver:parentWindow:handler:)]) {
		[_plugCode beginImportSampleAtURL:sampleURL driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		handler(MADOrderNotImplemented, nil);
	}
}

- (void)beginExportSample:(PPSampleObject*)aSamp toURL:(NSURL*)sampleURL driver:(PPDriver *)driver parentDocument:(PPDocument *)document handler:(PPPlugErrorBlock)handler
{
	if (![_plugCode conformsToProtocol:@protocol(PPSampleExportPlugin)]) {
		handler(MADOrderNotImplemented);
		return;
	}
	
	if ([_plugCode hasUIForExport]) {
		PPSampleObject *aSampObj;
		MADErr iErr = [_plugCode exportSample:aSampObj toURL:sampleURL driver:driver];
		if (iErr != MADOrderNotImplemented) {
			handler(iErr);
			return;
		}
	}
	if ([_plugCode respondsToSelector:@selector(beginExportSample:toURL:driver:parentWindow:handler:)]) {
		[_plugCode beginExportSample:aSamp toURL:sampleURL driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		handler(MADOrderNotImplemented);
	}
}

@end
