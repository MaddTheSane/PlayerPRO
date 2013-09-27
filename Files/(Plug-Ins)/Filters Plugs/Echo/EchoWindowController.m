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


@interface EchoWindowController ()

@end

static Boolean getParams (SInt32 *p1, double *p2, PPInfoPlug *thePPInfoPlug)
{
	EchoWindowController *controller = [[EchoWindowController alloc] initWithWindowNibName:@"DepthController"];
	controller.echoDelay = *p1;
	controller.echoStrength = *p2;
	
	NSInteger retVal = [controller runAsModal];
	
	if (retVal == NSOffState) {
		return FALSE;
	} else {
		*p1 = controller.echoDelay;
		*p2 = controller.echoStrength;
		return TRUE;
	}
}

@implementation EchoWindowController

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

@end

#define timeConvert		22254 //≈22KHZ

#if 0
static int checkMax (int v)
{
	if( v >= 127) return 127;
	else if( v <= -127 ) return -127;
	else return v;
}
#endif

static OSErr mainEcho(void			*unused,
					  sData			*theData,
					  long			SelectionStart,
					  long			SelectionEnd,
					  PPInfoPlug		*thePPInfoPlug,
					  short			StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	SInt32 i, length, temp1, temp2, pDelay = 250;
	double pStrength = 0.50;
	
	if (getParams (&pDelay, &pStrength, thePPInfoPlug))
	{
		length = (int)(SelectionEnd - SelectionStart - 1);
		
		pDelay = (pDelay * timeConvert) / 1000;	//convert ms to samples
		
		switch( theData->amp)
		{
			case 8:
			{
				Ptr	orgPtr = (theData->data) + SelectionStart, destPtr = orgPtr + pDelay;
				
				for( i = 0; i < (length - pDelay); i++)
				{
					temp1 = *orgPtr++;
					temp1 = (pStrength * temp1);
					
					temp2 = *destPtr;
					
					temp1 += temp2;
					
					if( temp1 >= 127) temp1 = 127;	// overflow ?
					else if( temp1 <= -128 ) temp1 = -128;
					
					*destPtr++ = temp1;
				}
			}
				break;
				
			case 16:
			{
				short	*orgPtr = (short*) theData->data + (SelectionStart / 2),
				*destPtr = orgPtr + pDelay;
				
				for( i = 0; i < length / 2 - pDelay; i++)
				{
					temp1 = *orgPtr++;
					temp1 = (pStrength * temp1);
					
					temp2 = *destPtr;
					
					temp1 += temp2;
					
					if( temp1 >= (short)0x7FFF) temp1 = 0x7FFF;	// overflow ?
					else if( temp1 <= (short)0x8000 ) temp1 = (short)0x8000;
					
					*destPtr++ = temp1;
				}
			}
				break;
		}
	}
	
	return noErr;
}

// DA609751-C4B0-4814-BAE7-2B82CA59E64E
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xDA, 0x60, 0x97, 0x51, 0xC4, 0xB0, 0x48, 0x14, 0xBA, 0xE7, 0x2B, 0x82, 0xCA, 0x59, 0xE6, 0x4E)

#define PLUGMAIN mainEcho
#define PLUGINFACTORY EchoFactory

#include "CFPlugin-FilterBridge.c"
