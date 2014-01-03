//
//  ComplexFadeController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "ComplexFadeController.h"
//#import "CompFadeNoteFormatter.h"

@interface ComplexFadeController ()

@end

static short NSStringToNote(NSString *myTT)
{
	short		Oct;
	NSString	*val1 = [myTT substringWithRange:[myTT rangeOfComposedCharacterSequencesForRange:NSMakeRange(0, 1)]];
	NSString	*val2 = [myTT length] >= 3 ? [myTT substringWithRange:[myTT rangeOfComposedCharacterSequencesForRange:NSMakeRange(1, 1)]] : @" ";
	NSString	*val3 = [myTT substringWithRange:[myTT rangeOfComposedCharacterSequencesForRange:NSMakeRange( [myTT length] >= 3 ? 2 : 1, 1)]];
	Oct = [val3 intValue];
	Oct *= 12;
	
	//	0	1	 2	 3	 4	 5	 6	 7 	 8	 9	 10	 11
	//	C-  C#   D-  D#  E-  F-  F#  G-  G#  A-  A#  B-
	if ([val1 compare:@"C" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)]) {
		Oct += 0;
	} else if ([val1 compare:@"D" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)]) {
		Oct += 2;
	} else if ([val1 compare:@"E" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)]) {
		Oct += 4;
	} else if ([val1 compare:@"F" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)]) {
		Oct += 5;
	} else if ([val1 compare:@"G" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)]) {
		Oct += 7;
	} else if ([val1 compare:@"A" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)]) {
		Oct += 9;
	} else if ([val1 compare:@"B" options:(NSCaseInsensitiveSearch | NSWidthInsensitiveSearch)]) {
		Oct += 11;
	} else
		Oct = 0xFF;
	
	if (Oct != 0xFF)
	{
		if ([val2 isEqualToString:@"#"])
			Oct++;
		if (Oct >= 96)
			Oct = 0xFF;
		if (Oct < 0)
			Oct = 0xFF;
	}
	
	return Oct;
}

static unsigned int NSStringToHex(NSString *str)
{
	NSScanner *tmpScanner = [[NSScanner alloc] initWithString:str];
	unsigned int tmpVal = 0;
	if (![tmpScanner scanHexInt:&tmpVal])
		return 0;
	if (tmpVal == UINT32_MAX) {
		return 0;
	}
	
	return tmpVal;
}

@implementation ComplexFadeController
@synthesize fadeType;

- (BOOL)validateSettings;
{
	return NO;
}

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		isMultipleIstanceSafe = YES;
		dispatch_block_t tmp = ^{
			
		};
		self.plugBlock = tmp;
	}
	
	return self;
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	
	
}

- (IBAction)changeFadeType:(id)sender
{
	self.fadeType = [sender tag];
	switch (self.fadeType) {
		case fadeInstrument:
			
			break;
			
		case fadeArgument:
			
			break;
			
		case fadeNote:
			
			break;
			
		case fadeVolume:
			
			break;
	}
}

- (IBAction)okOrCancel:(id)sender
{
	if ([sender tag] == 1) {
		[super okOrCancel:sender];
		return;
	}
	if (![self validateSettings]) {
		
	} else
		[super okOrCancel:sender];
}

@end

static OSErr mainCompFade(void *unused, Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	ComplexFadeController *controller = [[ComplexFadeController alloc] initWithWindowNibName:@"ComplexFadeController" infoPlug:thePPInfoPlug];
	controller.thePcmd = myPcmd;
	controller.fadeType = fadeInstrument;
	
	return [controller runAsSheet];
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x2C, 0xE9, 0x02, 0x81, 0xE2, 0xC2, 0x47, 0x5A, 0xA0, 0xF0, 0xB9, 0x0C, 0x64, 0x1E, 0xAE, 0xB1)
//2CE90281-E2C2-475A-A0F0-B90C641EAEB1
#define PLUGINFACTORY CompFadeFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainCompFade //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"
