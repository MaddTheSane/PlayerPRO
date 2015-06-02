//
//  PPCoreInstrumentPlugBridge.m
//  PPCoreInstrumentPlugBridge
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPCoreInstrumentPlugBridge.h"
#import "PPInstrumentPlugBridgeObject.h"
#import "PPInstrumentPlugBridgeHelper.h"
#import "ARCBridge.h"

#define kMadPlugIsSampleKey @"MADPlugIsSample"

static int nativeCPUArch()
{
#if __x86_64__
	return NSBundleExecutableArchitectureX86_64;
#elif __i386__
	return NSBundleExecutableArchitectureI386;
#else
#error unknown architecture!
	return 0;
#endif
}

typedef NS_OPTIONS(unsigned char, MADPlugCapabilities) {
	PPMADCanDoNothing	= 0,
	PPMADCanImport		= 1 << 0,
	PPMADCanExport		= 1 << 1,
	PPMADCanDoBoth		= PPMADCanImport | PPMADCanExport
};

void **PPINLoadPlug(CFBundleRef tempBundleRef)
{
	CFArrayRef	factories		= NULL;
	BOOL		foundInterface	= NO;
	void		**formatPlugA	= NULL;
	
	CFPlugInRef plugToTest = CFBundleGetPlugIn(tempBundleRef);
	
	if (!plugToTest)
		return NULL;
	
	//  See if this plug-in implements the Test type.
	factories = CFPlugInFindFactoriesForPlugInTypeInPlugIn(kPlayerPROInstrumentPlugTypeID, plugToTest);
	
	if (factories != NULL) {
		CFIndex	factoryCount, index;
		
		factoryCount = CFArrayGetCount(factories);
		if (factoryCount > 0) {
			for (index = 0; (index < factoryCount) && (foundInterface == false); index++) {
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

@interface PPCoreInstrumentPlugBridge ()
- (void)isBundleAtURLIsInstrumentBundle:(NSURL*)bundle withReply:(void (^)(BOOL isPlug, BOOL isInstrument, BOOL isImport))reply;
@end

@implementation PPCoreInstrumentPlugBridge
@synthesize plugIns;

static inline OSType NSStringToOSType(NSString *CFstri)
{
	return UTGetOSTypeFromString(BRIDGE(CFStringRef, CFstri));
}

__unused static inline NSString* OSTypeToNSString(OSType theOSType) 
{
	return CFBridgingRelease(UTCreateStringForOSType(theOSType));
}

- (void)checkBundleAtURLIsSampleBundle:(NSURL *)bundle withReply:(void (^)(BOOL))reply
{
	[self isBundleAtURLIsInstrumentBundle:bundle withReply:^(BOOL isPlug, BOOL isInstrument, BOOL isImport) {
		reply(isPlug && !isInstrument);
	}];
}

- (void)checkBundleAtURLIsInstrumentBundle:(NSURL *)bundle withReply:(void (^)(BOOL))reply
{
	[self isBundleAtURLIsInstrumentBundle:bundle withReply:^(BOOL isPlug, BOOL isInstrument, BOOL isImport) {
		reply(isPlug && isInstrument);
	}];
}

- (PPInstrumentPlugBridgeObject*)plugInAtURL:(NSURL*)url
{
	for (PPInstrumentPlugBridgeObject *obj in plugIns) {
		if ([obj.bundleFile.bundleURL isEqual:url]) {
			return obj;
		}
	}
	
	//Make sure we have the bundle object set up, as the helper may have been invalidated.
	NSBundle *bundle = [NSBundle bundleWithURL:url];
	PPInstrumentPlugBridgeObject *obj = nil;
	if (bundle != nil) {
		obj = [[PPInstrumentPlugBridgeObject alloc] initWithBundle:bundle];
	}
	if (obj != nil) {
		[plugIns addObject:obj];
	}
	
	return AUTORELEASEOBJ(obj);
}

- (void)isBundleAtURLIsInstrumentBundle:(NSURL*)bundle withReply:(void (^)(BOOL isPlug, BOOL isInstrument, BOOL isImport))reply
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
		reply(NO, NO, NO);
		return;
	}
	CFBundleRef postBundle = CFBundleCreate(kCFAllocatorDefault, (__bridge CFURLRef)bundle);
	IUnknownVTbl **plugVtbl = (IUnknownVTbl**)PPINLoadPlug(postBundle);
	if (!plugVtbl) {
		
		CFRelease(postBundle);
		reply(NO, NO, NO);
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
		reply((capabilities & PPMADCanImport) == PPMADCanImport, !isSample, (capabilities & PPMADCanImport) == PPMADCanImport);
		
		if ((capabilities & PPMADCanImport) == PPMADCanImport) {
			PPInstrumentPlugBridgeObject *obj = [[PPInstrumentPlugBridgeObject alloc] initWithBundle:preBundle];
			if (obj != nil) {
				[plugIns addObject:obj];
			}
			RELEASEOBJ(obj);
		}
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
		plugIns = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void)canImportFileAtURL:(NSURL*)aFile bundleURL:(NSURL*)bundle withReply:(void (^)(BOOL))reply
{
	PPInstrumentPlugBridgeObject* obj = [self plugInAtURL:bundle];
	if (obj != nil) {
		BOOL toRet = [obj canLoadFileAtURL:aFile];
		reply(toRet);
		return;
	}
	
	reply(NO);
}

static void freeIns(InstrData* curIns, sData **samples) {
	for (short i = 0; i < curIns->numSamples; i++) {
		if (samples[i] != NULL) {
			if (samples[i]->data != NULL) {
				free(samples[i]->data);
				samples[i]->data = NULL;
			}
			free(samples[i]);
			samples[i] = NULL;
		}
	}
}

- (void)beginImportFileAtURL:(NSURL*)aFile withBundleURL:(NSURL*)bundle instrumentData:(NSData*)insData instrumentNumber:(short)insNum reply:(void (^)(MADErr error, NSData *outInsData))reply
{
	PPInstrumentPlugBridgeObject *plug = [self plugInAtURL:bundle];
	if (plug == nil) {
		reply(MADCannotFindPlug, nil);
		return;
	}
	
	if (plug.sample) {
		reply(MADOrderNotImplemented, nil);
		return;
	}
	sData **samples;
	InstrData *instrument = MADDataToInstrument(insData, &samples);
	
	NSData *ourData = nil;
	short aSamp = -1;
	MADErr iErr = [plug importURL:aFile instrument:instrument sampleArray:samples sampleIndex:&aSamp];
	
	if (iErr == noErr) {
		ourData = MADInstrumentToData(instrument, samples);
	}
	reply(iErr, ourData);
	freeIns(instrument, samples);
	free(samples);
	free(instrument);
	RELEASEOBJ(ourData);
}

- (void)beginImportFileAtURL:(NSURL*)aFile withBundleURL:(NSURL*)bundle sampleData:(NSData*)sampData instrumentNumber:(short)insNum sampleNumber:(short)sampNum reply:(void (^)(MADErr error, NSData *outInsData, BOOL newSample))reply
{
	PPInstrumentPlugBridgeObject *plug = [self plugInAtURL:bundle];
	if (plug == nil) {
		reply(MADCannotFindPlug, nil, NO);
		return;
	}
	
	if (!plug.sample) {
		reply(MADOrderNotImplemented, nil, NO);
		return;
	}
	sData **samples = calloc(sizeof(sData*), MAXINSTRU);
	InstrData *instrument = calloc(sizeof(InstrData), 1);
	
	instrument->no = 0;
	sData *sample = MADDataToSample(sampData);
	if (sample != NULL) {
		instrument->numSamples = 1;
		samples[0] = sample;
	} else {
		instrument->numSamples = 0;
	}
	
	NSData *ourData = nil;
	short aSamp = sampNum != -1 ? 0 : -1;
	short preSamp = sampNum;
	MADErr iErr = [plug importURL:aFile instrument:instrument sampleArray:samples sampleIndex:&aSamp];
	
	if (iErr == noErr) {
		ourData = MADSampleToData(samples[aSamp]);
	}
	reply(iErr, ourData, aSamp != preSamp);
	
	freeIns(instrument, samples);
	free(samples);
	free(instrument);
	RELEASEOBJ(ourData);
}

+ (instancetype)sharedImporter
{
	static dispatch_once_t onceToken;
	static PPCoreInstrumentPlugBridge *shared;
	dispatch_once(&onceToken, ^{
		shared = [PPCoreInstrumentPlugBridge new];
	});
	return shared;
}


- (BOOL)listener:(NSXPCListener *)listener shouldAcceptNewConnection:(NSXPCConnection *)newConnection {
	// This method is where the NSXPCListener configures, accepts, and resumes a new incoming NSXPCConnection.
	
	// Configure the connection.
	// First, set the interface that the exported object implements.
	newConnection.exportedInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPCoreInstrumentPlugBridgeProtocol)];
	
	// Next, set the object that the connection exports. All messages sent on the connection to this service will be sent to the exported object to handle. The connection retains the exported object.
	newConnection.exportedObject = self;
	
	// Resuming the connection allows the system to deliver more incoming messages.
	[newConnection resume];
	
	// Returning YES from this method tells the system that you have accepted this connection. If you want to reject the connection for some reason, call -invalidate on the connection and return NO.
	return YES;
}

@end
