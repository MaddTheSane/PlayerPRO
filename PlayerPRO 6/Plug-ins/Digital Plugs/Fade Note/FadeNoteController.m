//
//  FadeNoteController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "FadeNoteController.h"
#import <PlayerPROKit/PlayerPROKit.h>

#define kNoteCompareOptions (NSCaseInsensitiveSearch | NSWidthInsensitiveSearch | NSDiacriticInsensitiveSearch)

static inline short NSStringToNote(NSString *myTT)
{
	return [PPSampleObject noteFromString:myTT];
}

@implementation FadeNoteController
@synthesize thePcmd;

#if 0
- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		// Initialization code here.
	}
	
	return self;
}
#endif

- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	[self.toBox removeAllItems];
	[self.fromBox removeAllItems];

	for (int i = 1; i < 96; i++) {
		NSString *str = [PPSampleObject octaveNameFromNote:i];
		[self.toBox addItemWithObjectValue:str];
		[self.fromBox addItemWithObjectValue:str];
	}
	[self.toBox selectItemWithObjectValue:self.initialTo];
	[self.fromBox selectItemWithObjectValue:self.initialFrom];
}

- (BOOL)validateSettings;
{
	NSString *tB, *fB;
	tB = [self.toBox stringValue];
	fB = [self.fromBox stringValue];
	
	if (NSStringToNote(tB) == 0xFF || NSStringToNote(fB) == 0xFF) {
		return NO;
	} else {
		return YES;
	}
}

- (IBAction)okay:(id)sender
{
	if (![self validateSettings]) {
		NSBeep();
		NSAlert *badSettings = [[NSAlert alloc] init];
		badSettings.messageText = @"Invalid Value";
		badSettings.informativeText = @"There is one or more invalid value.";
		[badSettings addButtonWithTitle:@"OK"];
		
		[[self window] beginCriticalSheet:badSettings.window completionHandler:^(NSModalResponse returnCode) {
			//
		}];
	} else {
		short from, to;
		Cmd *myCmd;
		
		from = NSStringToNote([self.fromBox stringValue]);
		to = NSStringToNote([self.toBox stringValue]);
		
		NSAssert(from != 0xFF && to != 0xFF, @"We should have checked for valid values already!");
		
		for (short track = 0; track < thePcmd->tracks; track++) {
			for (short row = 0; row < thePcmd->length; row++) {
				myCmd = MADGetCmd(row, track, thePcmd);
				
				if (thePcmd->length > 1)			// no zero div !!
					myCmd->note	= from + ((to-from) * row) / (thePcmd->length - 1);
				
				// my fade command : 0x10 min vol, 0x50 : max vol
				// Refer to MAD description for more informations
			}
		}
		[(NSApplication*)NSApp endSheet:self.window];
		_currentBlock(MADNoErr);
	}
}

- (IBAction)cancel:(id)sender
{
	[(NSApplication*)NSApp endSheet:self.window];
	_currentBlock(MADUserCanceledErr);
}

@end
