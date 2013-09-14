//
//  ComplexFadeController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "ComplexFadeController.h"

@interface ComplexFadeController ()

@end

@implementation ComplexFadeController

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
    isMultipleIstanceSafe = YES;
	[super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

@end

static OSErr mainCompFade(void *unused, Pcmd *myPcmd, PPInfoPlug *thePPInfoPlug)
{
	return MADOrderNotImplemented;
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x2C, 0xE9, 0x02, 0x81, 0xE2, 0xC2, 0x47, 0x5A, 0xA0, 0xF0, 0xB9, 0x0C, 0x64, 0x1E, 0xAE, 0xB1)
//2CE90281-E2C2-475A-A0F0-B90C641EAEB1
#define PLUGINFACTORY CompFadeFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainCompFade //The old main function, renamed please

#include "CFPlugin-DigitalBridge.c"
