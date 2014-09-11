//
//  ComplexFadeController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "ComplexFadeController.h"
//#import "CompFadeNoteFormatter.h"

#define kNoteCompareOptions (NSCaseInsensitiveSearch | NSWidthInsensitiveSearch | NSDiacriticInsensitiveSearch)

@interface ComplexFadeController ()

@end

#if 0
static int NSStringToHex(NSString *str)
{
	NSScanner *tmpScanner = [[NSScanner alloc] initWithString:str];
	unsigned int tmpVal = 0;
	if (![tmpScanner scanHexInt:&tmpVal])
		return -1;
	else if (tmpVal == UINT32_MAX || tmpVal > 0xFF) {
		return -1;
	}
	
	return tmpVal;
}
#endif

@implementation ComplexFadeController
@synthesize fadeType;

- (BOOL)validateSettings;
{
	return NO;
}

#if 0
- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
	}
	
	return self;
}
#endif

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

- (IBAction)okay:(id)sender
{
		if (![self validateSettings]) {
		NSBeep();
		NSAlert *badSettings = [[NSAlert alloc] init];
		badSettings.messageText = @"Invalid Value";
		badSettings.informativeText = @"There is one or more invalid value.";
		[badSettings addButtonWithTitle:@"OK"];
		
		[badSettings beginSheetModalForWindow:[self window] completionHandler:^(NSModalResponse returnCode) {
			;//Do nothing right now
		}];
		}

}

- (IBAction)cancel:(id)sender
{
	
}

@end

#if 0
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
#endif 
