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
			
		};
		self.plugBlock = tmpBlock;
	}
	
	return self;
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

@end

static OSErr mainFadeNote(void *unused, Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	return MADOrderNotImplemented;
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x95, 0x45, 0xDB, 0x21, 0x5A, 0xDE, 0x49, 0xDC, 0x97, 0x17, 0x09, 0x3D, 0x09, 0xEC, 0x4D, 0x39)
//9545DB21-5ADE-49DC-9717-093D09EC4D39
#define PLUGINFACTORY FadeNoteFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainFadeNote //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"
