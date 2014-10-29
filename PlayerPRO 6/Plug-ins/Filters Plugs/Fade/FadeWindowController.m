//
//  FadeWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "FadeWindowController.h"
#include <PlayerPROCore/PlayerPROCore.h>

@implementation FadeWindowController
@synthesize theData;

#if 0
- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		
	}
	
	return self;
}
#endif

- (IBAction)okay:(id)sender
{
	NSMutableData *ourData = theData.data.mutableCopy;
	long	i, per;
	double	from = self.fadeFrom, to = self.fadeTo, temp;
	
	
	char	*Sample8Ptr = ourData.mutableBytes;
	short	*Sample16Ptr = (short*)Sample8Ptr;
	
	switch (theData.amplitude) {
		case 8:
			Sample8Ptr += self.selectionRange.location;
			
			for (i = 0; i < self.selectionRange.length; i++) {
				temp = *Sample8Ptr;
				if (temp >= 0x80)
					temp -= 0xFF;
				
				per = from + ((to-from) * i) / (self.selectionRange.length);
				
				temp *= per;
				temp /= 100L;
				if (temp >= 127)
					temp = 127;
				else if (temp <= -127)
					temp = -127;
				
				*Sample8Ptr = temp;
				
				if (self.stereoMode) {
					Sample8Ptr++;
					i++;
				}
				
				Sample8Ptr++;
			}
			break;
			
		case 16:
			Sample16Ptr += self.selectionRange.location / 2;							// Div 2, because it's in bytes !!!
			
			for (i = 0; i < (self.selectionRange.length) / 2; i++) {	// Div 2, because it's in bytes !!!
				temp = *Sample16Ptr;
				
				per = from + ((to-from) * i) / ((self.selectionRange.length) / 2);
				
				temp *= per;
				temp /= 100L;
				
				if (temp >= (short)0x7FFF)
					temp = 0x7FFF;	// overflow ?
				else if (temp <= (short)0x8000)
					temp = (short)0x8000;
				
				*Sample16Ptr = temp;
				
				if (self.stereoMode) {
					Sample16Ptr++;
					i++;
				}
				Sample16Ptr++;
			}
			break;
	}

	theData.data = ourData;
	
	[(NSApplication*)NSApp endSheet:self.window];
	_currentBlock(MADNoErr);
}

- (IBAction)cancel:(id)sender
{
	[(NSApplication*)NSApp endSheet:self.window];
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
