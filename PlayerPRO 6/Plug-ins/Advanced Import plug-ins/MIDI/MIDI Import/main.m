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
#import <AppKit/AppKit.h>

static void midiDebugFunc(short line, const char *file, const char *text)
{
	@autoreleasepool {
		NSLog(@"%s:%u, error text:%s!", file, line, text);
		NSInteger alert = NSRunAlertPanel(NSLocalizedString(@"MyDebugStr_Error", @"Error"),
										  NSLocalizedString(@"MyDebugStr_MainText", @"The Main text to display"),
										  NSLocalizedString(@"MyDebugStr_Quit", @"Quit"), NSLocalizedString(@"MyDebugStr_Continue", @"Continue"),
										  NSLocalizedString(@"MyDebugStr_Debug", @"Debug"), text);
		switch (alert) {
			case NSAlertAlternateReturn:
				break;
				
			case NSAlertOtherReturn:
				NSCAssert(NO, @"Chose to go to debugger.");
				break;
				
			case NSAlertDefaultReturn:
				NSLog(@"Choosing to fail!");
			default:
				abort();
				break;
		}
	}
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
