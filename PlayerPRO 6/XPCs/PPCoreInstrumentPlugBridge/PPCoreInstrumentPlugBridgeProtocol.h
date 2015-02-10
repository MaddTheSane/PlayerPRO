//
//  PPCoreInstrumentPlugBridgeProtocol.h
//  PPCoreInstrumentPlugBridge
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

// The protocol that this service will vend as its API. This header file will also need to be visible to the process hosting the service.
@protocol PPCoreInstrumentPlugBridgeProtocol <NSObject>
- (void)checkBundleAtURLIsInstrumentBundle:(NSURL*)bundle withReply:(void (^)(BOOL isPlug))reply;
- (void)canImportFileAtURL:(NSURL*)aFile bundleURL:(NSURL*)bundle withReply:(void (^)(BOOL))reply;

- (void)beginImportFileAtURL:(NSURL*)aFile withBundleURL:(NSURL*)bundle instrumentData:(NSData*)insData instrumentNumber:(short)insNum reply:(void (^)(MADErr error, NSData *outInsData))reply;
- (void)beginImportFileAtURL:(NSURL*)aFile withBundleURL:(NSURL*)bundle sampleData:(NSData*)sampData instrumentNumber:(short)insNum sampleNumber:(short)sampNum reply:(void (^)(MADErr error, NSData *outInsData, BOOL newSample))reply;

- (void)checkBundleAtURLIsSampleBundle:(NSURL*)bundle withReply:(void (^)(BOOL isPlug))reply;
@end

/*
 To use the service from an application or other process, use NSXPCConnection to establish a connection to the service by doing something like this:

     _connectionToService = [[NSXPCConnection alloc] initWithServiceName:@"PPCoreInstrumentPlugBridge"];
     _connectionToService.remoteObjectInterface = [NSXPCInterface interfaceWithProtocol:@protocol(StringModifing)];
     [_connectionToService resume];

Once you have a connection to the service, you can use it like this:

     [[_connectionToService remoteObjectProxy] upperCaseString:@"hello" withReply:^(NSString *aString) {
         // We have received a response. Update our text field, but do it on the main thread.
         NSLog(@"Result string was: %@", aString);
     }];

 And, when you are finished with the service, clean up the connection like this:

     [_connectionToService invalidate];
*/
