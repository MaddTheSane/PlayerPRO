//
//  main.m
//  PPCoreInstrumentPlugBridge
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import <Foundation/Foundation.h>
#import "PPCoreInstrumentPlugBridge.h"

static void midiDebugFunc(short line, const char *file, const char *text)
{
	NSLog(@"%s:%u, error text:%s!", file, line, text);
	// Invalidate our listener...
	[[NSXPCListener serviceListener] invalidate];
	// ...sleep for a bit...
	usleep(5000);
	// ...then crash!
	abort();
}

int main(int argc, const char *argv[])
{
	MADRegisterDebugFunc(midiDebugFunc);
    // Create the delegate for the service.
    PPCoreInstrumentPlugBridge *delegate = [PPCoreInstrumentPlugBridge sharedImporter];
    
    // Set up the one NSXPCListener for this service. It will handle all incoming connections.
    NSXPCListener *listener = [NSXPCListener serviceListener];
    listener.delegate = delegate;
    
    // Resuming the serviceListener starts this service. This method does not return.
    [listener resume];
    return 0;
}
