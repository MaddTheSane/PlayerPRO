//
//  PPSamplePlugCompatObject.m
//  PPMacho
//
//  Created by C.W. Betts on 2/10/15.
//
//

#import "PPSamplePlugCompatObject.h"
#import "PPSamplePlugObject-private.h"
#import "PPCoreInstrumentPlugBridgeProtocol.h"
#import "PPInstrumentPlugBridgeHelper.h"

@interface PPSamplePlugCompatObject ()
@property BOOL is32Bit;
@property (strong) NSXPCConnection *connectionToService;
- (instancetype)initWithBundleNoInit:(NSBundle *)tempBundle;
@end


@implementation PPSamplePlugCompatObject
- (instancetype)initWithBundleNoInit:(NSBundle *)tempBundle
{
	return [self initWithBundle:tempBundle];
}

- (void)generateConnection
{
	self.connectionToService = [[NSXPCConnection alloc] initWithServiceName: self.is32Bit ? @"net.sourceforge.playerpro.PPCoreInstrumentPlugBridge32" : @"net.sourceforge.playerpro.PPCoreInstrumentPlugBridge"];
	self.connectionToService.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPCoreInstrumentPlugBridgeProtocol)];
	
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-retain-cycles"
	// We can ignore the retain cycle warning because a) the retain taken by the
	// invalidation handler block is released by us setting it to nil when the block
	// actually runs, and b) the retain taken by the block passed to -addOperationWithBlock:
	// will be released when that operation completes and the operation itself is deallocated
	// (notably self does not have a reference to the NSBlockOperation).
	dispatch_block_t errorHandler = ^{
		// If the connection gets invalidated then, on the main thread, nil out our
		// reference to it.  This ensures that we attempt to rebuild it the next time around.
		self.connectionToService.invalidationHandler = nil;
		self.connectionToService.interruptionHandler = nil;
		[[NSOperationQueue mainQueue] addOperationWithBlock:^{
			self.connectionToService = nil;
			NSLog(@"Connection invalidated for plug-in %@ (%p)", self, self);
		}];
	};
	self.connectionToService.invalidationHandler = errorHandler;
	self.connectionToService.interruptionHandler = errorHandler;
#pragma clang diagnostic pop
}

- (id<PPCoreInstrumentPlugBridgeProtocol>)remoteConnectionProxy
{
	if (!_connectionToService) {
		[self generateConnection];
		
		[_connectionToService resume];
	}
	
	return [_connectionToService remoteObjectProxyWithErrorHandler:^(NSError *error) {
		NSLog(@"Error: %@", error);
	}];
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
		[self generateConnection];
		[_connectionToService resume];
		__block BOOL toRet = NO;
		
		dispatch_semaphore_t ourSemaphore = dispatch_semaphore_create(0);
		dispatch_async(dispatch_get_global_queue(0, 0), ^{
			[[self remoteConnectionProxy]
			 checkBundleAtURLIsSampleBundle:tempBundle.bundleURL
			 withReply:^(BOOL isPlug) {
				 toRet = isPlug;
				 dispatch_semaphore_signal(ourSemaphore);
			 }];
		});
		dispatch_semaphore_wait(ourSemaphore, dispatch_time(DISPATCH_TIME_NOW, 3 * NSEC_PER_SEC));
		if (toRet == NO) {
			return nil;
		}
		self.canImport = YES;
		self.canExport = NO;
	}
	return self;
}

- (void)dealloc
{
	[_connectionToService suspend];
	self.connectionToService.invalidationHandler = nil;
	self.connectionToService.interruptionHandler = nil;
	[_connectionToService invalidate];
}

- (MADErr)playInstrumentAtURL:(NSURL *)aSample driver:(PPDriver *)driver
{
	return MADOrderNotImplemented;
}

-(void)beginImportSampleAtURL:(NSURL *)sampleURL driver:(PPDriver *)driver parentDocument:(PPDocument *)document handler:(void (^)(NSError*, PPSampleObject *))handler
{
	NSData *aDat = PPSampleToData([PPSampleObject new]);
	[[self remoteConnectionProxy] beginImportFileAtURL:sampleURL withBundleURL:self.file.bundleURL sampleData:aDat instrumentNumber:0 sampleNumber: -1 reply:^(MADErr error, NSData *outSampData, BOOL isNewSample) {
		if (error == MADNoErr) {
			handler(nil, PPDataToSample(outSampData));
		} else {
			handler([NSError errorWithDomain:PPMADErrorDomain code:error userInfo:nil], nil);
		}
	}];
}

- (void)beginExportSample:(PPSampleObject *)aSamp toURL:(NSURL *)sampleURL driver:(PPDriver *)driver parentDocument:(PPDocument *)document handler:(PPPlugErrorBlock)handler
{
	handler([NSError errorWithDomain:PPMADErrorDomain code:MADOrderNotImplemented userInfo:@{NSLocalizedFailureReasonErrorKey: @"Legacy plug-ins cannot export."}]);
}

- (BOOL)canImportFileAtURL:(NSURL *)fileURL;
{
	//Force generation of resumed connection proxy if one's not available.
	[self remoteConnectionProxy];
	[_connectionToService resume];
	__block BOOL toRet = NO;
	
	dispatch_semaphore_t ourSemaphore = dispatch_semaphore_create(0);
	dispatch_async(dispatch_get_global_queue(0, 0), ^{
		[[self remoteConnectionProxy]
		 canImportFileAtURL:fileURL bundleURL:self.file.bundleURL
		 withReply:^(BOOL ourReply) {
			 toRet = ourReply;
			 usleep(5);
			 dispatch_semaphore_signal(ourSemaphore);
		 }];
	});
	dispatch_semaphore_wait(ourSemaphore, dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC));
	[_connectionToService suspend];
	
	return toRet;
}

@end
