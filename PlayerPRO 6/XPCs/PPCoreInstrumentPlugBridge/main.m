//
//  main.m
//  PPCoreInstrumentPlugBridge
//
//  Created by C.W. Betts on 9/11/14.
//
//

#import <Foundation/Foundation.h>
#import "PPCoreInstrumentPlugBridge.h"

int main(int argc, const char *argv[])
{
    // Create the delegate for the service.
    PPCoreInstrumentPlugBridge *delegate = [PPCoreInstrumentPlugBridge sharedImporter];
    
    // Set up the one NSXPCListener for this service. It will handle all incoming connections.
    NSXPCListener *listener = [NSXPCListener serviceListener];
    listener.delegate = delegate;
    
    // Resuming the serviceListener starts this service. This method does not return.
    [listener resume];
    return 0;
}
