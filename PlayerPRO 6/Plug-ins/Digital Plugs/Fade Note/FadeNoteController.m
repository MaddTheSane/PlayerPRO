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

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		// Initialization code here.
		isMultipleIstanceSafe = YES;
		dispatch_block_t tmpBlock = ^{
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
		};
		self.plugBlock = tmpBlock;
	}
	
	return self;
}

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

- (IBAction)okOrCancel:(id)sender
{
	if ([sender tag] == 1) {
		[super okOrCancel:sender];
		return;
	}
	if (![self validateSettings]) {
		NSBeep();
		NSAlert *badSettings = [[NSAlert alloc] init];
		badSettings.messageText = @"Invalid Value";
		badSettings.informativeText = @"There is one or more invalid value.";
		[badSettings addButtonWithTitle:@"OK"];
		
		[badSettings beginSheetModalForWindow:[self window] completionHandler:^(NSModalResponse returnCode) {
			;//Do nothing right now
		}];
	} else
		[super okOrCancel:sender];
}

@end

static OSErr mainFadeNote(void *unused, Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	FadeNoteController *controller = [[FadeNoteController alloc] initWithWindowNibName:@"FadeNoteController" infoPlug:thePPInfoPlug];
	controller.thePcmd = myPcmd;
	controller.initialFrom = @"C 3";
	controller.initialTo = @"C 7";
	
	return [controller runAsSheet];
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x95, 0x45, 0xDB, 0x21, 0x5A, 0xDE, 0x49, 0xDC, 0x97, 0x17, 0x09, 0x3D, 0x09, 0xEC, 0x4D, 0x39)
//9545DB21-5ADE-49DC-9717-093D09EC4D39
#define PLUGINFACTORY FadeNoteFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainFadeNote //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"
