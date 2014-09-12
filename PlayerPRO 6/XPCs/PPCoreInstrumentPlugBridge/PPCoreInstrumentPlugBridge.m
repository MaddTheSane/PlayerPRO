//
//  PPCoreInstrumentPlugBridge.m
//  PPCoreInstrumentPlugBridge
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPCoreInstrumentPlugBridge.h"

NSString * const kMadPlugIsSampleKey = @"MADPlugIsSample";

static int nativeCPUArch()
{
#if __x86_64__
	return NSBundleExecutableArchitectureX86_64;
#elif __i386__
	return NSBundleExecutableArchitectureI386;
#else
	return 0;
#endif
}

typedef NS_OPTIONS(unsigned char, MADPlugCapabilities) {
	PPMADCanDoNothing	= 0,
	PPMADCanImport		= 1 << 0,
	PPMADCanExport		= 1 << 1,
	PPMADCanDoBoth		= PPMADCanImport | PPMADCanExport
};

#if 0
void **GetCOMPlugInterface(CFBundleRef tempBundleRef, CFUUIDRef TypeUUID, CFUUIDRef InterfaceUUID)
{
	CFArrayRef	factories		= NULL;
	BOOL		foundInterface	= NO;
	void		**formatPlugA	= NULL;
	
	CFPlugInRef plugToTest = CFBundleGetPlugIn(tempBundleRef);
	
	if (!plugToTest)
		return NULL;
	
	//  See if this plug-in implements the Test type.
	factories = CFPlugInFindFactoriesForPlugInTypeInPlugIn(TypeUUID, plugToTest);
	
	if (factories != NULL) {
		CFIndex	factoryCount, index;
		
		factoryCount = CFArrayGetCount(factories);
		if (factoryCount > 0) {
			for (index = 0 ; (index < factoryCount) && (foundInterface == false) ; index++) {
				CFUUIDRef factoryID;
				
				//  Get the factory ID for the first location in the array of IDs.
				factoryID = (CFUUIDRef) CFArrayGetValueAtIndex(factories, index);
				if (factoryID) {
					IUnknownVTbl **iunknown = NULL;
					
					//  Use the factory ID to get an IUnknown interface. Here the plug-in code is loaded.
					iunknown = (IUnknownVTbl **)CFPlugInInstanceCreate(kCFAllocatorDefault, factoryID, TypeUUID);
					
					if (iunknown) {
						//  If this is an IUnknown interface, query for the test interface.
						(*iunknown)->QueryInterface(iunknown, CFUUIDGetUUIDBytes(InterfaceUUID), (LPVOID *)(&formatPlugA));
						
						// Now we are done with IUnknown
						(*iunknown)->Release(iunknown);
						
						if (formatPlugA) {
							// We found the interface we need
							foundInterface = true;
						}
					}
				}
			}
		} else {
			//You can ignore the Clang static warning of incorrect decrement here.
			CFRelease(factories);
			return NULL;
		}
	} else
		return NULL;
	
	//You can ignore the Clang static warning of incorrect decrement here.
	CFRelease(factories);
	
	return formatPlugA;
}
#define PPINLoadPlug(theBundle) (PPInstrumentPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROInstrumentPlugTypeID, kPlayerPROInstrumentPlugInterfaceID)
#else
//#define PPINLoadPlug(theBundle) (PPInstrumentPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROInstrumentPlugTypeID, kPlayerPROInstrumentPlugInterfaceID)
IUnknownVTbl **PPINLoadPlug(CFBundleRef tempBundleRef)
{
	CFArrayRef	factories		= NULL;
	BOOL		foundInterface	= NO;
	IUnknownVTbl		**formatPlugA	= NULL;
	
	CFPlugInRef plugToTest = CFBundleGetPlugIn(tempBundleRef);
	
	if (!plugToTest)
		return NULL;
	
	//  See if this plug-in implements the Test type.
	factories = CFPlugInFindFactoriesForPlugInTypeInPlugIn(kPlayerPROInstrumentPlugTypeID, plugToTest);
	
	if (factories != NULL) {
		CFIndex	factoryCount, index;
		
		factoryCount = CFArrayGetCount(factories);
		if (factoryCount > 0) {
			for (index = 0 ; (index < factoryCount) && (foundInterface == false) ; index++) {
				CFUUIDRef factoryID;
				
				//  Get the factory ID for the first location in the array of IDs.
				factoryID = (CFUUIDRef) CFArrayGetValueAtIndex(factories, index);
				if (factoryID) {
					IUnknownVTbl **iunknown = NULL;
					
					//  Use the factory ID to get an IUnknown interface. Here the plug-in code is loaded.
					iunknown = (IUnknownVTbl **)CFPlugInInstanceCreate(kCFAllocatorDefault, factoryID, kPlayerPROInstrumentPlugTypeID);
					
					if (iunknown) {
						//  If this is an IUnknown interface, query for the test interface.
						(*iunknown)->QueryInterface(iunknown, CFUUIDGetUUIDBytes(kPlayerPROInstrumentPlugInterfaceID), (LPVOID *)(&formatPlugA));
						
						// Now we are done with IUnknown
						(*iunknown)->Release(iunknown);
						
						if (formatPlugA) {
							// We found the interface we need
							foundInterface = true;
						}
					}
				}
			}
		} else {
			//You can ignore the Clang static warning of incorrect decrement here.
			CFRelease(factories);
			return NULL;
		}
	} else
		return NULL;
	
	//You can ignore the Clang static warning of incorrect decrement here.
	CFRelease(factories);
	
	return formatPlugA;
}
#endif
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

@implementation PPCoreInstrumentPlugBridge

__unused static inline OSType NSStringToOSType(NSString *CFstri)
{
	return UTGetOSTypeFromString((__bridge CFStringRef)(CFstri));
}

__unused static inline NSString* OSTypeToNSString(OSType theOSType) 
{
	return CFBridgingRelease(UTCreateStringForOSType(theOSType));
}

- (void)checkBundleAtURLIsInstrumentBundle:(NSURL*)bundle withReply:(void (^)(BOOL isPlug, BOOL isInstrument, BOOL isImport, BOOL isExport))reply
{
	NSBundle *preBundle = [NSBundle bundleWithURL:bundle];
	NSArray *archs = [preBundle executableArchitectures];
	BOOL hasValidArch = NO;
	for (NSNumber *arch in archs) {
		if ([arch isEqual:@(nativeCPUArch())]) {
			hasValidArch = YES;
			break;
		}
	}
	if (hasValidArch == NO) {
		reply(NO, NO, NO, NO);
		return;
	}
	CFBundleRef postBundle = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef)bundle);
	IUnknownVTbl **plugVtbl = PPINLoadPlug(postBundle);
	if (!plugVtbl) {
		
		CFRelease(postBundle);
		reply(NO, NO, NO, NO);
	} else {
		NSDictionary *infoPlist = [preBundle infoDictionary];
		id DictionaryTemp;
		id canImportValue, canExportValue;
		canImportValue = [infoPlist valueForKey:(__bridge NSString*)kMadPlugDoesImport];
		canExportValue = [infoPlist valueForKey:(__bridge NSString*)kMadPlugDoesExport];
		MADPlugCapabilities capabilities = PPMADCanDoNothing;
		if (canImportValue || canExportValue) {
			if (canImportValue) {
				if (getBoolFromId(canImportValue))
					capabilities = PPMADCanImport;
			}
			if (canExportValue) {
				if (getBoolFromId(canExportValue))
					capabilities |= PPMADCanExport;
			}
			
		} else {
			OSType tempRetType = 0;
			DictionaryTemp = [infoPlist valueForKey:(__bridge NSString*)kMadPlugModeKey];
			if ([DictionaryTemp isKindOfClass:[NSString class]]) {
				tempRetType = NSStringToOSType(DictionaryTemp);
			} else if ([DictionaryTemp isKindOfClass:numClass]) {
				tempRetType = [(NSNumber*)DictionaryTemp unsignedIntValue];
			}
			
			switch (tempRetType) {
				case MADPlugImport:
					capabilities = PPMADCanImport;
					break;
					
				case MADPlugExport:
					capabilities = PPMADCanExport;
					break;
					
				case MADPlugImportExport:
					capabilities = PPMADCanDoBoth;
					break;
					
				default:
					capabilities = PPMADCanDoNothing;
					break;
			}
		}
		
		BOOL isSample = NO;
		DictionaryTemp = [infoPlist valueForKey:kMadPlugIsSampleKey];
		if ([DictionaryTemp isKindOfClass:numClass] || [DictionaryTemp isKindOfClass:strClass]) {
			isSample = [DictionaryTemp boolValue];
		}
		
		(*plugVtbl)->Release(plugVtbl);
		CFRelease(postBundle);
		reply(capabilities != PPMADCanDoNothing, !isSample, (capabilities & PPMADCanImport) == PPMADCanImport, (capabilities & PPMADCanExport) == PPMADCanExport);
	}
}

- (instancetype)init
{
	if (self = [super init]) {
		static dispatch_once_t onceToken;
		dispatch_once(&onceToken, ^{
			strClass = [NSString class];
			numClass = [NSNumber class];
		});
	}
	return self;
}

- (void)canImportFileAtURL:(NSURL*)aFile bundleURL:(NSURL*)bundle withReply:(void (^)(BOOL))reply
{
	
}

@end
