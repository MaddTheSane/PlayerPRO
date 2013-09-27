//
//  SamplingRateWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "SamplingRateWindowController.h"
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

@interface SamplingRateWindowController ()

@end

static Ptr ConvertSampleC4SPD( Ptr src, unsigned int srcSize, short amp, int srcC4SPD, int dstC4SPD, Boolean stereo, size_t *newsize)
{
#define LRVAL	3L
	
	short						*src16 = (short*) src, *dst16;
	char						*src8 = (char*) src, *dst8;
	Ptr							dst;
	int						newSize, tempL, tempR;
	unsigned  int			x, left, right, pos;
	
	srcC4SPD /= 100;
	dstC4SPD /= 100;
	
	newSize = (srcSize * dstC4SPD) / srcC4SPD;
	
	*newsize = newSize;
	
	dst = malloc( newSize);
	if( dst == NULL) return NULL;
	
	dst16 = (short*) dst;
	dst8 = (char*) dst;
	
	switch( amp)
	{
		case 8:
			for( x = 0; x < newSize; x++)
			{
				pos			= (x * srcC4SPD << LRVAL) / dstC4SPD;
				right		= pos & ((1 << LRVAL)-1);
				left		= (1 << LRVAL) - right;
				
				if( stereo)
				{
					pos >>= LRVAL;
					pos /= 2;
					pos *= 2;
					
					if( 1 + pos >= srcSize) {}
					else tempL = (left * src8[ pos] + right * src8[ 2 + pos]) >> LRVAL;
					
					dst8[ x] = tempL;
					
					x++;
					
					if( 3 + pos >= srcSize) {}
					else tempR = (left * src8[ 1 + pos] + right * src8[ 3 + pos]) >> LRVAL;
					
					dst8[ x] = tempR;
				}
				else
				{
					pos >>= LRVAL;
					
					if( pos < 0 ||  1 + pos >= srcSize) {}
					else tempL = (left * src8[ pos] + right * src8[ 1 + pos]) >> LRVAL;
					
					dst8[ x] = tempL;
				}
			}
			break;
			
		case 16:
			for( x = 0; x < newSize/2; x++)
			{
				pos			= (x * srcC4SPD << LRVAL) / dstC4SPD;
				right		= pos & ((1 << LRVAL)-1);
				left		= (1 << LRVAL) - right;
				
				if( stereo)
				{
					pos >>= LRVAL;
					pos /= 2;
					pos *= 2;
					
					if( 1 + pos >= srcSize/2) {}
					else tempL = (left * src16[ pos] + right * src16[ 2 + pos]) >> LRVAL;
					
					dst16[ x] = tempL;
					
					x++;
					
					if( 3 + pos >= srcSize/2) {}
					else tempL = (left * src16[ 1 + pos] + right * src16[ 3 + pos]) >> LRVAL;
					
					dst16[ x] = tempL;
				}
				else
				{
					pos >>= LRVAL;
					
					if( 1 + pos >= srcSize/2) {}
					else tempL = (left * src16[ pos] + right * src16[ 1 + pos]) >> LRVAL;
					
					dst16[ x] = tempL;
				}
			}
			break;
	}
	
	return dst;
}

@implementation SamplingRateWindowController

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

static OSErr mainSampRate(void			*unused,
						  sData			*theData,
						  long			SelectionStart,
						  long			SelectionEnd,
						  PPInfoPlug	*thePPInfoPlug,
						  short			StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	
	SamplingRateWindowController *controller = [[SamplingRateWindowController alloc] initWithWindowNibName:@"SamplingRateWindowController"];
	
	controller.currentRate = controller.changedRate = theData->c2spd;
	
	NSInteger retval = [controller runAsModal];
	if( retval != NSOffState)
	{
		int	newFreq = controller.changedRate;
		Ptr		newPtr;
		size_t newPtrSize = 0;
				
		newPtr = ConvertSampleC4SPD(	theData->data,
									theData->size,
									theData->amp,
									theData->c2spd,
									newFreq,
									theData->stereo,
									&newPtrSize);
		
		theData->loopBeg = (theData->loopBeg * (newFreq/100)) / (long) (theData->c2spd/100);
		theData->loopSize = (theData->loopSize * (newFreq/100)) / (long) (theData->c2spd/100);
		
		if( newPtr != NULL)
		{
			free( theData->data);
			theData->data		= newPtr;
			theData->size		= (SInt32)newPtrSize;
			theData->c2spd	= newFreq;
		}
		
		
		if( theData->loopBeg < 0) { theData->loopSize += theData->loopBeg;	theData->loopBeg = 0;}
		if( theData->loopBeg > theData->size) {theData->loopBeg = 0;	theData->loopSize = 0;}
		if( theData->loopSize < 0) theData->loopSize = 0;
		if( theData->loopBeg + theData->loopSize > theData->size) theData->loopSize = theData->size - theData->loopBeg;
	}
	
	return noErr;
}

// 81D156A0-6737-4D5F-BE63-5BA48F527276
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x81, 0xD1, 0x56, 0xA0, 0x67, 0x37, 0x4D, 0x5F, 0xBE, 0x63, 0x5B, 0xA4, 0x8F, 0x52, 0x72, 0x76)

#define PLUGMAIN mainSampRate
#define PLUGINFACTORY SampRateFactory

#include "CFPlugin-FilterBridge.c"
