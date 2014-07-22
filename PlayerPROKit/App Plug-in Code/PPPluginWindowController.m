//
//  MADPluginWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/13/13.
//
//

#import "PPPluginWindowController.h"

NSString * const PPPlugInSheetDidEnd = @"PlayerPROKit Plugin Sheet Did End";
NSString * const PPPlugReturnCode = @"PlayerPROKit Return Code";

@interface PPPluginWindowController ()
@property BOOL isRunningModal;
@end

@implementation PPPluginWindowController
@synthesize infoPlug;
@synthesize plugBlock;

#if 0
- (instancetype)initWithWindowNibName:(NSString *)windowNibName
{
	return self = [super initWithWindowNibName:windowNibName];
}
#endif

- (instancetype)initWithWindowNibName:(NSString *)windowNibName infoPlug:(PPInfoPlug *)ip
{
	if (self = [super initWithWindowNibName:windowNibName]) {
		self.infoPlug = ip;
	}
	return self;
}

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		// Initialization code here.
	}
	
	return self;
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	if (isMultipleIstanceSafe && infoPlug) {
		parentWindow = (__bridge NSWindow*)(infoPlug->NSWindow);
		infoPlug->OutWindowController = CFBridgingRetain(self);
	}
}

- (IBAction)okOrCancel:(id)sender
{
	if (_isRunningModal) {
		[NSApp stopModalWithCode:([sender tag] == 1) ? NSOffState : NSOnState];
	} else {
		[parentWindow endSheet:self.window returnCode:[sender tag] == 1 ? NSOffState : NSOnState];
	}
}

- (MADErr)runAsModal
{
	self.isRunningModal = YES;
	NSInteger retVal = [NSApp runModalForWindow:self.window];
	if (self.plugBlock) {
		plugBlock();
	}
	[self close];
	return retVal;
}

- (MADErr)runAsSheet
{
	if (isMultipleIstanceSafe) {
		if (!parentWindow) {
			self.isRunningModal = YES;
			NSInteger retVal = [NSApp runModalForWindow:self.window];
			if (self.plugBlock) {
				plugBlock();
			}
			[self close];
			return retVal;
		}
		
		self.isRunningModal = NO;
		[parentWindow beginSheet:self.window completionHandler:^(NSModalResponse returnCode) {
			if (returnCode == NSOnState) {
				plugBlock();
			}
			[[NSNotificationCenter defaultCenter] postNotificationName:PPPlugInSheetDidEnd object:self userInfo:@{PPPlugReturnCode: @(returnCode)}];
			[self close];

		}];
		return MADIsRunningSheet;
	} else
		return MADOrderNotImplemented;
}

@end
