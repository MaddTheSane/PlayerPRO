//
//  main.m
//  MIDI Import
//
//  Created by C.W. Betts on 8/6/14.
//
//

#include <xpc/xpc.h>
#include <Foundation/Foundation.h>
#include "PPMIDIImporter.h"

@interface ServiceDelegate : NSObject <NSXPCListenerDelegate>
@end

@implementation ServiceDelegate

- (BOOL)listener:(NSXPCListener *)listener shouldAcceptNewConnection:(NSXPCConnection *)newConnection {
	// This method is where the NSXPCListener configures, accepts, and resumes a new incoming NSXPCConnection.
	
	// Configure the connection.
	// First, set the interface that the exported object implements.
	newConnection.exportedInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPMIDIImportHelper)];
	
	// Next, set the object that the connection exports. All messages sent on the connection to this service will be sent to the exported object to handle. The connection retains the exported object.
	PPMIDIImporter *exportedObject = [PPMIDIImporter new];
	newConnection.exportedObject = exportedObject;
	
	// Resuming the connection allows the system to deliver more incoming messages.
	[newConnection resume];
	
	// Returning YES from this method tells the system that you have accepted this connection. If you want to reject the connection for some reason, call -invalidate on the connection and return NO.
	return YES;
}

@end


int main(int argc, const char *argv[])
{
	// Get the singleton service listener object.
	ServiceDelegate *delegate = [ServiceDelegate new];
	
	// Configure the service listener with a delegate.
	NSXPCListener *listener = [NSXPCListener serviceListener];
	listener.delegate = delegate;
	
	// Resume the listener. At this point, NSXPCListener will take over the execution of this service, managing its lifetime as needed.
	[listener resume];
	return 0;
}
