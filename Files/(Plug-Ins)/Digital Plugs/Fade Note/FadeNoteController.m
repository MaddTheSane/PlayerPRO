//
//  FadeNoteController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "FadeNoteController.h"

static short NSStringToNote(NSString *myTT)
{
	if (!myTT || [myTT isEqualToString:@""] || [myTT isEqualToString:@"---"]) {
		return 0xFF;
	}
	
	short		Oct;
	NSString	*val1 = [myTT substringWithRange:[myTT rangeOfComposedCharacterSequencesForRange:NSMakeRange(0, 1)]];
	NSString	*val2 = [myTT length] >= 3 ? [myTT substringWithRange:[myTT rangeOfComposedCharacterSequencesForRange:NSMakeRange(1, 1)]] : @" ";
	NSString	*val3 = [myTT substringWithRange:[myTT rangeOfComposedCharacterSequencesForRange:NSMakeRange([myTT length] >= 3 ? 2 : 1, 1)]];
	Oct = [val3 intValue];
	Oct *= 12;
	
	//	0	1	 2	 3	 4	 5	 6	 7 	 8	 9	 10	 11
	//	C-  C#   D-  D#  E-  F-  F#  G-  G#  A-  A#  B-
	if ([val1 compare:@"C" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)] == NSOrderedSame) {
		Oct += 0;
	} else if ([val1 compare:@"D" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)] == NSOrderedSame) {
		Oct += 2;
	} else if ([val1 compare:@"E" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)] == NSOrderedSame) {
		Oct += 4;
	} else if ([val1 compare:@"F" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)] == NSOrderedSame) {
		Oct += 5;
	} else if ([val1 compare:@"G" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)] == NSOrderedSame) {
		Oct += 7;
	} else if ([val1 compare:@"A" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)] == NSOrderedSame) {
		Oct += 9;
	} else if ([val1 compare:@"B" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)] == NSOrderedSame) {
		Oct += 11;
	} else
		Oct = 0xFF;
	
	if (Oct != 0xFF) {
		if ([val2 isEqualToString:@"#"])
			Oct++;
		if (Oct >= 96)
			Oct = 0xFF;
		if (Oct < 0)
			Oct = 0xFF;
	}
	
	return Oct;
}

@interface FadeNoteController ()

@end

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
					myCmd = GetCmd(row, track, thePcmd);
					
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

+ (NSString*)octaveNameFromNote:(short)octNote
{
	const char NNames[][3] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};

	if (octNote > 95) {
		return @"---";
	}
	
	return [[NSString alloc] initWithFormat:@"%s%i", NNames[octNote % 12], octNote / 12];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	[self.toBox removeAllItems];
	[self.fromBox removeAllItems];

	for (int i = 1; i < 96; i++) {
		NSString *str = [FadeNoteController octaveNameFromNote:i];
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
	
	return [controller runAsModal];
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x95, 0x45, 0xDB, 0x21, 0x5A, 0xDE, 0x49, 0xDC, 0x97, 0x17, 0x09, 0x3D, 0x09, 0xEC, 0x4D, 0x39)
//9545DB21-5ADE-49DC-9717-093D09EC4D39
#define PLUGINFACTORY FadeNoteFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainFadeNote //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"
