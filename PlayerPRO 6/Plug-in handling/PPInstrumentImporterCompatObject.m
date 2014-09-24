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

#define PPINLoadPlug(theBundle) (PPInstrumentPlugin**)GetCOMPlugInterface(theBundle, kPlayerPROInstrumentPlugTypeID, kPlayerPROInstrumentPlugInterfaceID)

static Class strClass;
static Class numClass;

@interface PPInstrumentImporterCompatObject ()
@property BOOL is32Bit;
@end

@implementation PPInstrumentImporterCompatObject

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
		NSXPCConnection *_connectionToService = [[NSXPCConnection alloc] initWithServiceName: self.is32Bit ? @"PPCoreInstrumentPlugBridge32" : @"PPCoreInstrumentPlugBridge"];
		_connectionToService.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPCoreInstrumentPlugBridgeProtocol)];
		[_connectionToService resume];
		__block BOOL toRet = NO;
		
		dispatch_semaphore_t ourSemaphore = dispatch_semaphore_create(0);
		dispatch_async(dispatch_get_global_queue(0, 0), ^{
			[[_connectionToService remoteObjectProxy] checkBundleAtURLIsInstrumentBundle:tempBundle.bundleURL withReply:^(BOOL isPlug, BOOL isInstrument, BOOL isImport) {
				toRet = isPlug;
				self.sample = !isInstrument;
				dispatch_semaphore_signal(ourSemaphore);
				[_connectionToService invalidate];
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
	NSXPCConnection *_connectionToService = [[NSXPCConnection alloc] initWithServiceName: self.is32Bit ? @"PPCoreInstrumentPlugBridge32" : @"PPCoreInstrumentPlugBridge"];
	_connectionToService.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPCoreInstrumentPlugBridgeProtocol)];
	[_connectionToService resume];
	__block BOOL toRet = NO;
	
	dispatch_semaphore_t ourSemaphore = dispatch_semaphore_create(0);
	dispatch_async(dispatch_get_global_queue(0, 0), ^{
		[[_connectionToService remoteObjectProxy] canImportFileAtURL:fileURL bundleURL:self.file.bundleURL withReply:^(BOOL ourReply) {
			toRet = ourReply;
			usleep(5);
			dispatch_semaphore_signal(ourSemaphore);
			[_connectionToService invalidate];
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
	handler(MADOrderNotImplemented);
}

- (void)beginExportSampleToURL:(NSURL*)sampleURL instrument:(PPInstrumentObject*)InsHeader sample:(PPSampleObject*)sample sampleID:(short)sampleID driver:(PPDriver*)driver parentDocument:(PPDocument*)document handler:(PPPlugErrorBlock)handler
{
	handler(MADOrderNotImplemented);
}


@end
