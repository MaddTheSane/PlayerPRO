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

static xpc_object_t NSDictionaryToXPCDictionary(NSDictionary *theIn) XPC_RETURNS_RETAINED;

xpc_object_t NSDictionaryToXPCDictionary(NSDictionary *theIn)
{
	xpc_object_t xpcDict = xpc_dictionary_create(NULL, NULL, 0);
	
	for (NSString *key in theIn) {
		id object = [theIn objectForKey:key];
		if ([object isKindOfClass:[NSNumber class]]) {
			xpc_dictionary_set_int64(xpcDict, [key UTF8String], [(NSNumber*)object unsignedLongLongValue]);
		} else if ([object isKindOfClass:[NSString class]]) {
			xpc_dictionary_set_string(xpcDict, [key UTF8String], [(NSString*)object UTF8String]);
		} else {
			NSCAssert(NO, @"We really shouldn't have got here!");
		}
	}
	
	return xpcDict;
}

static void MIDIImport_peer_event_handler(xpc_connection_t peer, xpc_object_t event)
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
		const char *strPath = xpc_dictionary_get_string(event, kPPLocation);
		PPMIDIImportValue currentDict = xpc_dictionary_get_int64(event, kPPMIDICall);
		MADErr currErr = noErr;
		PPMIDIImporter *importer;
		@autoreleasepool {
			NSURL *currentURL = [NSURL fileURLWithFileSystemRepresentation:strPath isDirectory:NO relativeToURL:nil];
			importer = [[PPMIDIImporter alloc] initWithURL:currentURL];
		}
		switch (currentDict) {
			case PPTestMIDI:
			{
				currErr = [importer canImportMIDIFile];
				xpc_object_t toRet = xpc_dictionary_create_reply(event);
				xpc_dictionary_set_int64(toRet, kPPMIDIErr, currErr);
				xpc_connection_send_message(peer, toRet);
				xpc_release(toRet);
			}
				break;
				
			case PPInfoMIDI:
			{
				NSMutableDictionary *tmpInfoDict = [[NSMutableDictionary alloc] init];
				currErr = [importer getMIDIInfoToDictionary:tmpInfoDict];
				xpc_object_t xpcDict = NSDictionaryToXPCDictionary(tmpInfoDict);
				[tmpInfoDict release];
				xpc_object_t toRet = xpc_dictionary_create_reply(event);
				xpc_dictionary_set_value(toRet, kPPMIDIInfo, xpcDict);
				xpc_release(xpcDict);
				xpc_dictionary_set_int64(toRet, kPPMIDIErr, currErr);
				xpc_connection_send_message(peer, toRet);
				xpc_release(toRet);
			}
				break;
				
			case PPImportMIDI:
			{
				NSData *theDat = nil;
				@autoreleasepool {
					currErr = [importer importMIDIFileToData:&theDat];
					[theDat retain];
				}
				xpc_object_t toRet = xpc_dictionary_create_reply(event);
				xpc_dictionary_set_data(toRet, kPPMADData, theDat.bytes, theDat.length);
				[theDat release];
				xpc_dictionary_set_int64(toRet, kPPMIDIErr, currErr);
				xpc_connection_send_message(peer, toRet);
				xpc_release(toRet);
			}
				break;
				
			default:
				break;
		}
		[importer release];
	}
}

static void MIDIImport_event_handler(xpc_connection_t peer)
{
	// By defaults, new connections will target the default dispatch
	// concurrent queue.
	xpc_connection_set_event_handler(peer, ^(xpc_object_t event) {
		MIDIImport_peer_event_handler(peer, event);
	});
	
	// This will tell the connection to begin listening for events. If you
	// have some other initialization that must be done asynchronously, then
	// you can defer this call until after that initialization is done.
	xpc_connection_resume(peer);
}

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
	//PPRegisterDebugFunc(midiDebugFunc);
	xpc_main(MIDIImport_event_handler);
	return 0;
}
