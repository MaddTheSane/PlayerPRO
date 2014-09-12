//
//  PPCoreInstrumentPlugBridge.m
//  PPCoreInstrumentPlugBridge
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import "PPCoreInstrumentPlugBridge.h"

@implementation PPCoreInstrumentPlugBridge

- (void)checkBundleAtURLIsInstrumentBundle:(NSURL*)bundle withReply:(void (^)(BOOL isPlug, BOOL isInstrument))reply
{
	
}

// This implements the example protocol. Replace the body of this class with the implementation of this service's protocol.
- (void)upperCaseString:(NSString *)aString withReply:(void (^)(NSString *))reply {
    NSString *response = [aString uppercaseString];
    reply(response);
}

@end
