//
//  AmplitudeController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "AmplitudeController.h"
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

@interface AmplitudeController ()

@end

@implementation AmplitudeController

- (instancetype)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
		isMultipleIstanceSafe = YES;
		
        dispatch_block_t ampPlugBlock = ^{
			SInt32	i = 0, temp = 0, Inc = self.amplitudeAmount;
			char	*Sample8Ptr = theData->data;
			short	*Sample16Ptr = (short*) theData->data;

			switch( theData->amp)
			{
				case 8:
					Sample8Ptr += selectionStart;
					
					for (i = 0; i < selectionEnd - selectionStart; i++)
					{
						temp = *Sample8Ptr;
						if (temp >= 0x80) temp -= 0xFF;
						
						temp *= Inc;
						temp /= 100L;
						if (temp >= 127) temp = 127;
						else if (temp <= -127 ) temp = -127;
						
						*Sample8Ptr = temp;
						
						if (stereoMode)
						{
							Sample8Ptr++;
							i++;
						}
						
						Sample8Ptr++;
					}
					break;
					
				case 16:
					Sample16Ptr += selectionStart/2;						// Div 2, because it's in bytes !!!
					
					for (i = 0; i < (selectionEnd - selectionStart)/2; i++)	// Div 2, because it's in bytes !!!
					{
						temp = *Sample16Ptr;
						
						temp *= Inc;
						temp /= 100L;
						
						if (temp >= (short) 0x7FFF) temp = 0x7FFF;	// overflow ?
						else if (temp <= (short) 0x8000 ) temp = (short) 0x8000;
						
						*Sample16Ptr = temp;
						
						if (stereoMode)
						{
							Sample16Ptr++;
							i++;
						}
						
						Sample16Ptr++;
					}
					break;
			}

		};
		
		self.plugBlock = ampPlugBlock;
    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}
@end

static OSErr mainAmplitude(void			*unused,
						   sData		*theData,
						   long			SelectionStart,
						   long			SelectionEnd,
						   PPInfoPlug	*thePPInfoPlug,
						   short		StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	AmplitudeController *controller = [[AmplitudeController alloc] initWithWindowNibName:@"AmplitudeController" infoPlug:thePPInfoPlug];
	controller.theData = theData;
	controller.selectionStart = SelectionStart;
	controller.selectionEnd = SelectionEnd;
	controller.amplitudeAmount = 120;
	controller.stereoMode = StereoMode ? YES : NO;
	
	return [controller runAsSheet];
}

#define PLUGUUID (CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xF1, 0xE5, 0xEA, 0x0B, 0x1A, 0xBC, 0x4D, 0x5F, 0x89, 0xDF, 0x3C, 0x89, 0x75, 0x8E, 0xAC, 0x0E))
//F1E5EA0B-1ABC-4D5F-89DF-3C89758EAC0E

#define PLUGMAIN mainAmplitude
#define PLUGINFACTORY AmplitudeFactory

#include "CFPlugin-FilterBridge.c"
