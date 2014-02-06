//
//  EchoWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "EchoWindowController.h"
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

#define timeConvert 22254 //≈22KHZ

@implementation EchoWindowController

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		// Initialization code here.
		isMultipleIstanceSafe = YES;
		dispatch_block_t tmp = ^{
			int i, length, temp1, temp2, pDelay = self.echoDelay;
			double pStrength = self.echoStrength;
			length = (int)(selectionEnd - selectionStart - 1);
			
			pDelay = (pDelay * timeConvert) / 1000;	//convert ms to samples
			
			switch (theData->amp) {
				case 8:
				{
					Ptr	orgPtr = (theData->data) + selectionStart, destPtr = orgPtr + pDelay;
					
					for (i = 0; i < (length - pDelay); i++) {
						temp1 = *orgPtr++;
						temp1 = (pStrength * temp1);
						
						temp2 = *destPtr;
						temp1 += temp2;
						
						if (temp1 >= 127)
							temp1 = 127;	// overflow ?
						else if (temp1 <= -128)
							temp1 = -128;
						
						*destPtr++ = temp1;
					}
				}
					break;
					
				case 16:
				{
					short	*orgPtr = (short*) theData->data + (selectionStart / 2),
					*destPtr = orgPtr + pDelay;
					
					for (i = 0; i < length / 2 - pDelay; i++) {
						temp1 = *orgPtr++;
						temp1 = (pStrength * temp1);
						
						temp2 = *destPtr;
						temp1 += temp2;
						
						if (temp1 >= (short)0x7FFF)
							temp1 = 0x7FFF;	// overflow ?
						else if (temp1 <= (short)0x8000)
							temp1 = (short)0x8000;
						
						*destPtr++ = temp1;
					}
				}
					break;
			}
		};
		self.plugBlock = tmp;
	}
	
	return self;
}

#if 0
- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}
#endif

@end

static OSErr mainEcho(void *unused, sData *theData, long SelectionStart, long SelectionEnd, PPInfoPlug *thePPInfoPlug, short StereoMode)
{
	EchoWindowController *controller = [[EchoWindowController alloc] initWithWindowNibName:@"EchoWindowController" infoPlug:thePPInfoPlug];
	controller.echoStrength = 0.50;
	controller.echoDelay = 250;
	controller.theData = theData;
	controller.selectionStart = SelectionStart;
	controller.selectionEnd = SelectionEnd;
	controller.stereoMode = StereoMode ? YES : NO;
	
	return [controller runAsSheet];
}

// DA609751-C4B0-4814-BAE7-2B82CA59E64E
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xDA, 0x60, 0x97, 0x51, 0xC4, 0xB0, 0x48, 0x14, 0xBA, 0xE7, 0x2B, 0x82, 0xCA, 0x59, 0xE6, 0x4E)
#define PLUGMAIN mainEcho
#define PLUGINFACTORY EchoFactory

#include "CFPlugin-FilterBridge.c"
