//
//  FadeWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "FadeWindowController.h"
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
@interface FadeWindowController ()

@end

static Boolean getParams ( double *p1, double *p2, PPInfoPlug *thePPInfoPlug)
{
	FadeWindowController *controller = [[FadeWindowController alloc] initWithWindowNibName:@"FadeWindowController"];
	controller.fadeTo = *p1;
	controller.fadeFrom = *p2;
	
	NSInteger retVal = [NSApp runModalForWindow:controller.window];
	[controller close];
	
	if (retVal == NSOffState) {
		return FALSE;
	} else {
		*p1 = controller.fadeTo;
		*p2 = controller.fadeFrom;
		return TRUE;
	}
}

@implementation FadeWindowController

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
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (IBAction)okOrCancel:(id)sender
{
	[NSApp stopModalWithCode:([sender tag] == 1) ? NSOffState : NSOnState];
}

@end

static OSErr mainFade(void			*unused,
					  sData			*theData,
					  long			SelectionStart,
					  long			SelectionEnd,
					  PPInfoPlug	*thePPInfoPlug,
					  short			StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long		i, per;
	double		from, to, temp;
	Ptr			Sample8Ptr = theData->data;
	short		*Sample16Ptr = (short*) theData->data;
	
	to = 1.0;
	from = .70;
	
	if( getParams( &to, &from, thePPInfoPlug))
	{
		switch( theData->amp)
		{
			case 8:
				Sample8Ptr += SelectionStart;
				
				for( i = 0; i < SelectionEnd - SelectionStart; i++)
				{
					temp = *Sample8Ptr;
					if( temp >= 0x80) temp -= 0xFF;
					
					per = from + ((to-from) * i) / (SelectionEnd - SelectionStart);
					
					temp *= per;
					temp /= 100L;
					if( temp >= 127) temp = 127;
					else if( temp <= -127 ) temp = -127;
					
					*Sample8Ptr = temp;
					
					if( StereoMode)
					{
						Sample8Ptr++;
						i++;
					}
					
					Sample8Ptr++;
				}
				break;
				
			case 16:
				Sample16Ptr += SelectionStart/2;						// Div 2, because it's in bytes !!!
				
				for( i = 0; i < (SelectionEnd - SelectionStart)/2; i++)	// Div 2, because it's in bytes !!!
				{
					temp = *Sample16Ptr;
					
					per = from + ((to-from) * i) / ((SelectionEnd - SelectionStart)/2);
					
					temp *= per;
					temp /= 100L;
					
					if( temp >= (short) 0x7FFF) temp = 0x7FFF;	// overflow ?
					else if( temp <= (short) 0x8000 ) temp = (short) 0x8000;
					
					*Sample16Ptr = temp;
					
					if( StereoMode)
					{
						Sample16Ptr++;
						i++;
					}
					Sample16Ptr++;
				}
				break;
		}
	}
	return noErr;
}

// 47C646EE-2B4B-428B-9309-C65B75CBE7EF
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x47, 0xC6, 0x46, 0xEE, 0x2B, 0x4B, 0x42, 0x8B, 0x93, 0x09, 0xC6, 0x5B, 0x75, 0xCB, 0xE7, 0xEF)

#define PLUGMAIN mainFade
#define PLUGINFACTORY FadeFactory

#include "CFPlugin-FilterBridge.c"
