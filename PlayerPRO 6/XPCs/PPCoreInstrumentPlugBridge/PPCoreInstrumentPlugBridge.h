//
//  PPCoreInstrumentPlugBridge.h
//  PPCoreInstrumentPlugBridge
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import <Foundation/Foundation.h>
#import "PPCoreInstrumentPlugBridgeProtocol.h"

__private_extern void **PPINLoadPlug(CFBundleRef tempBundleRef);

// This object implements the protocol which we have defined. It provides the actual behavior for the service. It is 'exported' by the service to make it available to the process hosting the service over an NSXPCConnection.
@interface PPCoreInstrumentPlugBridge : NSObject <PPCoreInstrumentPlugBridgeProtocol>
{
	NSMutableArray *plugIns;
}
@property (retain) NSMutableArray *plugIns;

@end
