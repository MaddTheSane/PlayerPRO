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
#import "PPCoreInstrumentPlugBridgeProtocol.h"
#import "PPInstrumentPlugBridgeHelper.h"

#define PPINLoadPlug(theBundle) (PPInstrumentPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROInstrumentPlugTypeID, kPlayerPROInstrumentPlugInterfaceID)

static NSXPCConnection *sharedConnectionToService;
static NSXPCConnection *sharedConnectionToService32;

static void moveInfoOverToInstrumentObjectFrom(PPInstrumentObject* to, PPInstrumentObject* from) {
	
}

@interface PPInstrumentImporterCompatObject ()
@property BOOL is32Bit;
@property (weak) NSXPCConnection *connectionToService;
@end

@implementation PPInstrumentImporterCompatObject

+ (void)initialize
{
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		sharedConnectionToService = [[NSXPCConnection alloc] initWithServiceName:@"PPCoreInstrumentPlugBridge"];
		sharedConnectionToService.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPCoreInstrumentPlugBridgeProtocol)];
		
		sharedConnectionToService32 = [[NSXPCConnection alloc] initWithServiceName:@"PPCoreInstrumentPlugBridge32"];
		sharedConnectionToService32.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPCoreInstrumentPlugBridgeProtocol)];
	});
}

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
	
	if (self = [super initWithBundleNoInit:tempBundle]) {
		if (has32 && !has64) {
			self.is32Bit = YES;
		}
		_connectionToService = self.is32Bit ? sharedConnectionToService32 : sharedConnectionToService;
		[_connectionToService resume];
		__block BOOL toRet = NO;
		
		dispatch_semaphore_t ourSemaphore = dispatch_semaphore_create(0);
		dispatch_async(dispatch_get_global_queue(0, 0), ^{
			[[_connectionToService remoteObjectProxy] checkBundleAtURLIsInstrumentBundle:tempBundle.bundleURL withReply:^(BOOL isPlug, BOOL isInstrument, BOOL isImport) {
				toRet = isPlug;
				self.sample = !isInstrument;
				dispatch_semaphore_signal(ourSemaphore);
			}];
		});
		dispatch_semaphore_wait(ourSemaphore, dispatch_time(DISPATCH_TIME_NOW, 10000));
		if (toRet == NO) {
			return nil;
		}
		self.canImport = YES;
		self.canExport = NO;
	}
	return self;
}

- (BOOL)canImportFileAtURL:(NSURL *)fileURL
{
	[_connectionToService resume];
	__block BOOL toRet = NO;
	
	dispatch_semaphore_t ourSemaphore = dispatch_semaphore_create(0);
	dispatch_async(dispatch_get_global_queue(0, 0), ^{
		[[_connectionToService remoteObjectProxy] canImportFileAtURL:fileURL bundleURL:self.file.bundleURL withReply:^(BOOL ourReply) {
			toRet = ourReply;
			usleep(5);
			dispatch_semaphore_signal(ourSemaphore);
		}];
	});
	dispatch_semaphore_wait(ourSemaphore, dispatch_time(DISPATCH_TIME_NOW, 5000));

	return toRet;
}

- (MADErr)playSampleAtURL:(NSURL*)aSample driver:(PPDriver*)driver;
{
	return MADOrderNotImplemented;
}

- (void)beginImportSampleAtURL:(NSURL*)sampleURL instrument:(inout PPInstrumentObject*)InsHeader sample:(inout PPSampleObject*)sample sampleID:(inout short*)sampleID driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handler
{
	if (!self.sample) {
		NSData *aDat = PPInstrumentToData(InsHeader);
		[[_connectionToService remoteObjectProxy] beginImportFileAtURL:sampleURL withBundleURL:self.file.bundleURL instrumentData:aDat instrumentNumber:InsHeader.number reply:^(MADErr error, NSData *outInsData) {
			if (error == MADNoErr) {
				PPInstrumentObject *aRet = PPDataToInstrument(outInsData);
				moveInfoOverToInstrumentObjectFrom(InsHeader, aRet);
			}
			
			handler(error);
		}];
	} else {
		NSData *aDat = PPSampleToData(sample);
		[[_connectionToService remoteObjectProxy] beginImportFileAtURL:sampleURL withBundleURL:self.file.bundleURL sampleData:aDat instrumentNumber:InsHeader.number sampleNumber:sampleID ? *sampleID : 0 reply:^(MADErr error, NSData *outSampData, short newSampleNum) {
			if (error == MADNoErr) {
				*sampleID = newSampleNum;
				[InsHeader replaceObjectInSamplesAtIndex:newSampleNum withObject:PPDataToSample(outSampData)];
			}
			
			handler(error);
		}];
	}
}

- (void)beginExportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handler
{
	handler(MADOrderNotImplemented);
}

@end
