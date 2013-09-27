//
//  DepthController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "DepthController.h"
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

@interface DepthController ()

@end

@implementation DepthController

- (id)initWithWindow:(NSWindow *)window
{
    self = [super initWithWindow:window];
    if (self) {
		isMultipleIstanceSafe = YES;

		dispatch_block_t tmp = ^{
			SInt32			i, temp, Inc = self.depthAmmount;
			char			*Sample8Ptr = (char*) theData->data;
			short			*Sample16Ptr = ( short*) theData->data;
			
			if( Inc == 0) Inc = 1;
			switch( theData->amp)
			{
				case 8:
					Sample8Ptr += selectionStart;
					
					for( i = 0; i < selectionEnd - selectionStart; i++)
					{
						temp = *Sample8Ptr;
						
						temp *= Inc;
						temp /= 256;
						
						temp *= 256;
						temp /= Inc;
						
						*Sample8Ptr = temp;
						
						if( stereoMode)
						{
							Sample8Ptr++;
							i++;
						}
						
						Sample8Ptr++;
					}
					break;
					
				case 16:
					Sample16Ptr += selectionStart/2;						// Div 2, because it's in bytes !!!
					
					for( i = 0; i < (selectionEnd - selectionStart)/2; i++)	// Div 2, because it's in bytes !!!
					{
						temp = *Sample16Ptr;
						
						temp *= Inc;
						temp /= 0x0000FFFF;
						
						temp *= 0x0000FFFF;
						temp /= Inc;
						
						*Sample16Ptr = temp;
						
						if( stereoMode)
						{
							Sample16Ptr++;
							i++;
						}
						
						Sample16Ptr++;
					}
					break;
			}


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

@end

static OSErr mainDepth(void						*unused,
					   sData					*theData,
					   long					SelectionStart,
					   long					SelectionEnd,
					   PPInfoPlug				*thePPInfoPlug,
					   short					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	DepthController *controller = [[DepthController alloc] initWithWindowNibName:@"DepthController" infoPlug:thePPInfoPlug];
	controller.depthAmmount = 8;
	controller.theData = theData;
	controller.selectionStart = SelectionStart;
	controller.selectionEnd = SelectionEnd;
	controller.stereoMode = StereoMode ? YES : NO;
	
	return [controller runAsSheet];
}

// D7F6D8C0-FC86-48E4-A2B8-61BB681DE071
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xD7, 0xF6, 0xD8, 0xC0, 0xFC, 0x86, 0x48, 0xE4, 0xA2, 0xB8, 0x61, 0xBB, 0x68, 0x1D, 0xE0, 0x71)

#define PLUGMAIN mainDepth
#define PLUGINFACTORY DepthFactory

#include "CFPlugin-FilterBridge.c"
