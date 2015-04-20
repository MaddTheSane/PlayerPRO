//
//  main.m
//  MIDI Import
//
//  Created by C.W. Betts on 8/6/14.
//
//

#include <xpc/xpc.h>
#import <Foundation/Foundation.h>
#import "PPMIDIImporter.h"
#include <PlayerPROCore/PlayerPROCore.h>

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
	// Get the singleton service listener object.
	NSXPCListener *serviceListener = [NSXPCListener serviceListener];
	
	// Configure the service listener with a delegate.
	PPMIDIImporter *sharedZipper = [PPMIDIImporter sharedImporter];
	serviceListener.delegate = sharedZipper;
	
	// Resume the listener. At this point, NSXPCListener will take over the execution of this service, managing its lifetime as needed.
	[serviceListener resume];
	return 0;
}
