//
//  EchoWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "EchoWindowController.h"
#include <PlayerPROCore/PlayerPROCore.h>

#define timeConvert 22254 //≈22KHZ

@implementation EchoWindowController
@synthesize theData;

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		// Initialization code here.
	}
	
	return self;
}

- (IBAction)okay:(id)sender
{
	int i, length, temp1, temp2, pDelay = self.echoDelay;
	double pStrength = self.echoStrength;
	NSMutableData *ourData = [theData.data mutableCopy];
	Ptr ourPtr = ourData.mutableBytes;
	length = (int)(self.selectionRange.length - 1);
	
	pDelay = (pDelay * timeConvert) / 1000;	//convert ms to samples
	
	switch (theData.amplitude) {
		case 8:
		{
			Ptr	orgPtr = (ourPtr) + _selectionRange.location, destPtr = orgPtr + pDelay;
			
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
			short	*orgPtr = (short*) ourPtr + (_selectionRange.location / 2),
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
	
	theData.data = ourData;
	[_parentWindow endSheet:self.window];
	_currentBlock(MADNoErr);
}

- (IBAction)cancel:(id)sender
{
	[_parentWindow endSheet:self.window];
	_currentBlock(MADUserCanceledErr);
}

#if 0
- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}
#endif

@end
