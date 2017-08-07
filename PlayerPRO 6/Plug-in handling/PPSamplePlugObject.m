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
	
	NSString *insString = CFBridgingRelease(CFUUIDCreateString(kCFAllocatorDefault, kPlayerPROInstrumentPlugTypeID));
	NSUUID *insUUID = [[NSUUID alloc] initWithUUIDString:insString];
	
	for (NSString *key in plugInTypes) {
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

BOOL isOrderNotImplemented(NSError *err) {
	return ([err.domain isEqualToString:PPMADErrorDomain] && err.code == MADOrderNotImplemented);
}

- (void)beginImportSampleAtURL:(NSURL*)sampleURL driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(void (^)(NSError *theErr, PPSampleObject *aSample))handler
{
	if (![_plugCode conformsToProtocol:@protocol(PPSampleImportPlugin)]) {
		handler([NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:nil], nil);
		return;
	}
	
	if ([_plugCode hasUIForImport]) {
		PPSampleObject *aSampObj;
		NSError *err;
		if (![_plugCode importSampleAtURL:sampleURL sample:&aSampObj driver:driver error:&err]) {
			if (!isOrderNotImplemented(err)) {
				handler(err, aSampObj);
				return;
			}
		} else {
			handler(nil, aSampObj);
			return;
		}
	}
	if ([_plugCode respondsToSelector:@selector(beginImportSampleAtURL:driver:parentWindow:handler:)]) {
		[_plugCode beginImportSampleAtURL:sampleURL driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		handler([NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:nil], nil);
	}
}

- (void)beginExportSample:(PPSampleObject*)aSamp toURL:(NSURL*)sampleURL driver:(PPDriver *)driver parentDocument:(PPDocument *)document handler:(PPPlugErrorBlock)handler
{
	if (![_plugCode conformsToProtocol:@protocol(PPSampleExportPlugin)]) {
		handler([NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:nil]);
		return;
	}
	
	if ([_plugCode hasUIForExport]) {
		PPSampleObject *aSampObj;
		NSError *err;
		if (![_plugCode exportSample:aSampObj toURL:sampleURL driver:driver error:&err]) {
			if (!isOrderNotImplemented(err)) {
				handler(err);
				return;
			}
		} else {
			handler(nil);
			return;
		}
	}
	if ([_plugCode respondsToSelector:@selector(beginExportSample:toURL:driver:parentWindow:handler:)]) {
		[_plugCode beginExportSample:aSamp toURL:sampleURL driver:driver parentWindow:[document windowForSheet] handler:handler];
	} else {
		handler([NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:nil]);
	}
}

@end
