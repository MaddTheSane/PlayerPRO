//
//  main.m
//  StcfImporter
//
//  Created by C.W. Betts on 4/20/14.
//
//

#include <xpc/xpc.h>
#import <Foundation/Foundation.h>
#include <Carbon/Carbon.h>
#import "PPSTImporter.h"

static void StcfImporter_peer_event_handler(xpc_connection_t peer, xpc_object_t event) 
{
	xpc_type_t type = xpc_get_type(event);
	if (type == XPC_TYPE_ERROR) {
		if (event == XPC_ERROR_CONNECTION_INVALID) {
			// The client process on the other end of the connection has either
			// crashed or cancelled the connection. After receiving this error,
			// the connection is in an invalid state, and you do not need to
			// call xpc_connection_cancel(). Just tear down any associated state
			// here.
		} else if (event == XPC_ERROR_TERMINATION_IMMINENT) {
			// Handle per-connection termination cleanup.
		}
	} else {
		assert(type == XPC_TYPE_DICTIONARY);
		// Handle the message.
	}
}

static void StcfImporter_event_handler(xpc_connection_t peer) 
{
	// By defaults, new connections will target the default dispatch
	// concurrent queue.
	xpc_connection_set_event_handler(peer, ^(xpc_object_t event) {
		StcfImporter_peer_event_handler(peer, event);
	});
	
	// This will tell the connection to begin listening for events. If you
	// have some other initialization that must be done asynchronously, then
	// you can defer this call until after that initialization is done.
	xpc_connection_resume(peer);
}

int main(int argc, const char *argv[])
{
    // Get the singleton service listener object.
    NSXPCListener *serviceListener = [NSXPCListener serviceListener];
    
    // Configure the service listener with a delegate.
    PPSTImporter *sharedZipper = [PPSTImporter sharedImporter];
    serviceListener.delegate = sharedZipper;
    
    // Resume the listener. At this point, NSXPCListener will take over the execution of this service, managing its lifetime as needed.
    [serviceListener resume];
    
	return 0;
}
