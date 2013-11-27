//
//  MixWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "MixWindowController.h"
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

@interface MixWindowController ()

@end

@implementation MixWindowController

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


static OSErr mainMix(void			*unused,
					 sData			*theData,
					 long			SelectionStart,
					 long			SelectionEnd,
					 PPInfoPlug		*thePPInfoPlug,
					 short			StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long				lCntOrErr;
	OSErr				anErr;
	double gp1, gp2;
	
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
	
	
	
	if ( lCntOrErr > 0) {
		NSString * instrumentStr = [[NSString alloc] initWithCString:theData->name encoding:NSMacOSRomanStringEncoding];
		
		MixWindowController *controller = [[MixWindowController alloc] initWithWindowNibName:@"MixWindowController"];
		[controller.instrumentLabel setTitleWithMnemonic:instrumentStr];
		controller.mixOriginal = controller.mixPasteboard = 50;
		gp1 = 0.50;
		gp2 = 0.50;
		
		if ([NSApp runModalForWindow:controller.window] != NSOffState) {
			long	i, length1, length2, resultLength, temp1, temp2;
			Ptr		clipPtr, resultPtr;
			
			HLock( aHandle);
			GetScrapFlavorData( scrap, soundListRsrc, &lCntOrErr, *aHandle);
			HUnlock( aHandle);
			
			HLockHi(aHandle);
			
			clipPtr = *aHandle;
			i = *(shortPtr)clipPtr;
			clipPtr += 4;
			if ((i == 1) || (i == 2)) {
				if ((i == 2) || (*(shortPtr)clipPtr == sampledSynth)) {
					Ptr		worgPtr, wclipPtr, wresultPtr;
					long	temp, peak = 0;
					
					if (i == 1) {
						clipPtr += 20;
					} else  clipPtr += 18;
					length2 = *(longPtr)clipPtr;	// sample length
					clipPtr += 18;					// points to sample data
					
					length1 = theData->size - SelectionStart;
					
					resultLength = SelectionStart + max( length1, length2);
					resultPtr = NewPtr(resultLength);
					
					worgPtr = theData->data; wclipPtr = clipPtr;
					for (i = 0; i < SelectionStart; i++) // first pass to get max value.
					{
						temp1 = *worgPtr++;
						peak = max( peak, labs( gp1 * temp1));
					}
					for (i = 0; i < (resultLength - SelectionStart); i++)
					{
						if (i < length1) {
							temp1 = *worgPtr;
						} else temp1 = 0;
						if (i < length2) {
							temp2 = (*wclipPtr & 0xFF) - 0x80;
						} else temp2 = 0;
						
						peak = max( peak, labs((gp1 * temp1) + (gp2 * temp2)));
						
						worgPtr++;
						wclipPtr++;
					}
					
					if (peak) {
						peak = ((long)0x80 * 0x10000) / peak;
					}
					
					worgPtr = theData->data; wclipPtr = clipPtr - SelectionStart; wresultPtr = resultPtr;
					for (i = 0; i < resultLength; i++) // one more time, all together, now !!!
					{
						if (i < theData->size) {
							temp1 = *worgPtr;
						} else temp1 = 0;
						if ((i >= SelectionStart) && (i < (length2 + SelectionStart))) {
							temp2 = (*wclipPtr & 0xFF) - 0x80;
						} else temp2 = 0;
						
						temp = (gp1 * temp1) + (gp2 * temp2);
						temp = (peak * temp) / 0x10000;
						
						if (temp >= 127) temp = 127;	// overflow ?
						else if (temp <= -128 ) temp = -128;
						
						*wresultPtr = temp;
						worgPtr++;
						wclipPtr++;
						wresultPtr++;
					}
					
					DisposePtr(theData->data);			//all done
					theData->data = resultPtr;			//now switch
					theData->size = resultLength;		//Instrument Ptr
					
				} else NSRunAlertPanel(@"Unknown Format", @"The audio in the pasteboard is not recognized", nil, nil, nil);
			} else NSRunAlertPanel(@"Unknown Format", @"The audio in the pasteboard is not recognized", nil, nil, nil);
			
			HUnlock(aHandle);
			
		}
		
		DisposeHandle(aHandle);
		
	} else alertUser (1);
	
	return noErr;
}

// AE796F78-C31E-47B2-B86D-42EA6474B674
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xAE, 0x79, 0x6F, 0x78, 0xC3, 0x1E, 0x47, 0xB2, 0xB8, 0x6D, 0x42, 0xEA, 0x64, 0x74, 0xB6, 0x74)

#define PLUGMAIN mainMix
#define PLUGINFACTORY MixFactory

#include "CFPlugin-FilterBridge.c"
