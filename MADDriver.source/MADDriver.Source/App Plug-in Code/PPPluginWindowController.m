//
//  MADPluginWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/13/13.
//
//

#import "PPPluginWindowController.h"

NSString * const PPPlugInSheetDidEnd = @"MAD Plugin sheet did end";
NSString * const PPPlugReturnCode = @"MAD Return Code";

@interface PPPluginWindowController ()
@property BOOL isRunningModal;
@end

@implementation PPPluginWindowController
@synthesize infoPlug;
@synthesize plugBlock;

- (id)initWithWindowNibName:(NSString *)windowNibName infoPlug:(PPInfoPlug *)ip
{
	if (self = [super initWithWindowNibName:windowNibName]) {
		self.infoPlug = ip;
	}
	return self;
}

- (void)sheetDidEnd:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
	if (returnCode == NSOnState) {
		plugBlock();
	}
	[[NSNotificationCenter defaultCenter] postNotificationName:PPPlugInSheetDidEnd object:self.window userInfo:@{PPPlugReturnCode: @(returnCode)}];
	[self close];
}

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) {
		// Initialization code here.
	}
	
	return self;
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	if (isMultipleIstanceSafe) {
		parentWindow = (__bridge NSWindow*)(infoPlug->NSWindow);
		infoPlug->OutWindowController = CFBridgingRetain(self);
	}
}

- (IBAction)okOrCancel:(id)sender
{
	if (!_isRunningModal) {
		[NSApp endSheet:self.window returnCode:[sender tag] == 1 ? NSOffState : NSOnState];
	}else {
		[NSApp stopModalWithCode:([sender tag] == 1) ? NSOffState : NSOnState];
	}
}

- (OSErr)runAsModal
{
	self.isRunningModal = YES;
	NSInteger retVal = [NSApp runModalForWindow:self.window];
	[self close];
	return retVal;
}

- (OSErr)runAsSheet
{
	if (isMultipleIstanceSafe) {
		self.isRunningModal = NO;
		[NSApp beginSheet:self.window modalForWindow:parentWindow modalDelegate:self didEndSelector:@selector(sheetDidEnd:returnCode:contextInfo:) contextInfo:NULL];
		
		return MADIsRunningModal;
	} else
		return MADOrderNotImplemented;
}

@end
