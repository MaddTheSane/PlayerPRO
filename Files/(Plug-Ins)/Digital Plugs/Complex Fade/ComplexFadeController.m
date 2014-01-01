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

static short Text2Note( char *myTT)
{
	short		Oct;
	
	if (strlen(myTT) > 3) Oct = myTT[ 2] - 48;
	else Oct = myTT[ 1] - 48;
	
	Oct *= 12;
	
	//	0	1	 2	 3	 4	 5	 6	 7 	 8	 9	 10	 11
	//	C-  C#   D-  D#  E-  F-  F#  G-  G#  A-  A#  B-
	switch( myTT[0])
	{
		case 'C':
		case 'c':
			Oct += 0;
			break;
			
		case 'D':
		case 'd':
			Oct += 2;
			break;
			
		case 'E':
		case 'e':
			Oct += 4;
			break;
			
		case 'F':
		case 'f':
			Oct += 5;
			break;
			
		case 'G':
		case 'g':
			Oct += 7;
			break;
			
		case 'A':
		case 'a':
			Oct += 9;
			break;
			
		case 'B':
		case 'b':
			Oct += 11;
			break;
			
		default:
			Oct = 0xFF;
			break;
	}
	
	if (Oct != 0xFF)
	{
		if (myTT[ 2] == '#') Oct++;
		if (Oct >= 96) Oct = 0xFF;
		if (Oct < 0) Oct = 0xFF;
	}
	
	return( Oct);
}

#if 0
static void StringToHex( char *str, int *oct)
{
	if (str[ 2] >= 'A' && str[ 2] <= 'F') *oct = 10 + str[ 2] - 'A';
	if (str[ 2] >= '0' && str[ 2] <= '9') *oct = str[ 2] - '0';
	
	if (str[ 1] >= 'A' && str[ 1] <= 'F') *oct += (10 + str[ 1] - 'A')<<4;
	if (str[ 1] >= '0' && str[ 1] <= '9') *oct += (str[ 1] - '0')<<4;
}
#endif


@implementation ComplexFadeController
@synthesize fadeType;

- (BOOL)validateSettings;
{
	
}

- (instancetype)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
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
	}
}

- (IBAction)okOrCancel:(id)sender
{
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
