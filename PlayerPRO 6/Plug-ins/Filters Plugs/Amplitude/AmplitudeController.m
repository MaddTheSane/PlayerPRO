//
//  AmplitudeController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "AmplitudeController.h"
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PPSampleObject.h>
#import <PlayerPROKit/PPErrors.h>

@implementation AmplitudeController
@synthesize theData;

- (instancetype)initWithWindow:(NSWindow *)window
{
	if (self = [super initWithWindow:window]) {
		
	}
	
	return self;
}

- (IBAction)okay:(id)sender
{
	NSMutableData *ourData = theData.data.mutableCopy;
	int		i = 0, temp = 0, Inc = self.amplitudeAmount;
	char	*Sample8Ptr = ourData.mutableBytes;
	short	*Sample16Ptr = ourData.mutableBytes;
	
	switch(theData.amplitude) {
		case 8:
			Sample8Ptr += self.selectionRange.location;
			
			for (i = 0; i < self.selectionRange.length; i++) {
				temp = *Sample8Ptr;
				if (temp >= 0x80)
					temp -= 0xFF;
				
				temp *= Inc;
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
				
				temp *= Inc;
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
	
	[_parentWindow endSheet:self.window];
	_currentBlock(nil);
}

- (IBAction)cancel:(id)sender
{
	[_parentWindow endSheet:self.window];
	_currentBlock([NSError errorWithDomain:PPMADErrorDomain code:MADUserCancelledErr userInfo:nil]);
}

#if 0
- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}
#endif

@end
