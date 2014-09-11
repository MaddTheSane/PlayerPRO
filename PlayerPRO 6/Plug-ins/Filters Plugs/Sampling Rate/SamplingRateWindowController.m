//
//  SamplingRateWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "SamplingRateWindowController.h"
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/MADPlug.h>

#define LRVAL 3

@implementation SamplingRateWindowController

- (NSData*)convertSampleC4SPD:(PPSampleObject *)src newRate:(int)dstC4SPD
{
	NSData			*ourData = src.data;
	size_t			srcSize = ourData.length;
	const short		*src16 = ourData.bytes;
	short			*dst16;
	const char		*src8 = ourData.bytes;
	char			*dst8;
	Ptr				dst;
	int				tempL = 0, tempR = 0;
	size_t			newSize = 0;
	unsigned int	x = 0, left = 0, right = 0, pos = 0;
	short			amp = src.amplitude;
	int				srcC4SPD = src.c2spd;
	const BOOL		stereo = src.stereo;
	
	srcC4SPD /= 100;
	dstC4SPD /= 100;
	
	newSize = (srcSize * dstC4SPD) / srcC4SPD;
	
	NSMutableData *dstData = [[NSMutableData alloc] initWithLength:newSize];
	
	dst = dstData.mutableBytes;
	if (dst == NULL)
		return nil;
	
	dst16 = (short*)dst;
	dst8 = (char*)dst;
	
	switch (amp) {
		case 8:
			for (x = 0; x < newSize; x++) {
				pos		= (x * srcC4SPD << LRVAL) / dstC4SPD;
				right	= pos & ((1 << LRVAL)-1);
				left	= (1 << LRVAL) - right;
				
				if (stereo) {
					pos >>= LRVAL;
					pos /= 2;
					pos *= 2;
					
					if (1 + pos >= srcSize) {
						
					} else {
						tempL = (left * src8[pos] + right * src8[2 + pos]) >> LRVAL;
					}
					
					dst8[x] = tempL;
					
					x++;
					
					if (3 + pos >= srcSize) {
						
					} else {
						tempR = (left * src8[1 + pos] + right * src8[3 + pos]) >> LRVAL;
					}
					
					dst8[x] = tempR;
				} else {
					pos >>= LRVAL;
					
					if (1 + pos >= srcSize) {
						
					} else {
						tempL = (left * src8[pos] + right * src8[1 + pos]) >> LRVAL;
					}
					
					dst8[x] = tempL;
				}
			}
			break;
			
		case 16:
			for (x = 0; x < newSize / 2; x++) {
				pos		= (x * srcC4SPD << LRVAL) / dstC4SPD;
				right	= pos & ((1 << LRVAL)-1);
				left	= (1 << LRVAL) - right;
				
				if (stereo) {
					pos >>= LRVAL;
					pos /= 2;
					pos *= 2;
					
					if (1 + pos >= srcSize / 2) {
						
					} else
						tempL = (left * src16[pos] + right * src16[2 + pos]) >> LRVAL;
					
					dst16[x] = tempL;
					
					x++;
					
					if (3 + pos >= srcSize / 2) {
						
					} else
						tempL = (left * src16[1 + pos] + right * src16[3 + pos]) >> LRVAL;
					
					dst16[x] = tempL;
				} else {
					pos >>= LRVAL;
					
					if (1 + pos >= srcSize / 2) {
						
					} else tempL = (left * src16[pos] + right * src16[1 + pos]) >> LRVAL;
					
					dst16[x] = tempL;
				}
			}
			break;
	}
	
	return [dstData copy];
}

#if 0
- (instancetype)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) {
		//isMultipleIstanceSafe = YES;
		
		dispatch_block_t tmp = ^{
			int	newFreq = self.changedRate;
			Ptr		newPtr;
			size_t newPtrSize = 0;
			
			newPtr = ConvertSampleC4SPD(theData->data, theData->size, theData->amp, theData->c2spd, newFreq, theData->stereo, &newPtrSize);
			
			theData->loopBeg = (theData->loopBeg * (newFreq / 100)) / (long) (theData->c2spd / 100);
			theData->loopSize = (theData->loopSize * (newFreq / 100)) / (long) (theData->c2spd / 100);
			
			if (newPtr != NULL) {
				free(theData->data);
				theData->data	= newPtr;
				theData->size	= (int)newPtrSize;
				theData->c2spd	= newFreq;
			}
			
			
			if (theData->loopBeg < 0) {
				theData->loopSize += theData->loopBeg;
				theData->loopBeg = 0;
			}
			if (theData->loopBeg > theData->size) {
				theData->loopBeg = 0;
				theData->loopSize = 0;
			}
			if (theData->loopSize < 0)
				theData->loopSize = 0;
			if (theData->loopBeg + theData->loopSize > theData->size)
				theData->loopSize = theData->size - theData->loopBeg;
		};
	}
	
	return self;
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

static OSErr mainSampRate(void *unused, sData *theData, long SelectionStart, long SelectionEnd, PPInfoPlug *thePPInfoPlug, short StereoMode)
{
	
	SamplingRateWindowController *controller = [[SamplingRateWindowController alloc] initWithWindowNibName:@"SamplingRateWindowController" infoPlug:thePPInfoPlug];
	controller.currentRate = controller.changedRate = theData->c2spd;
	controller.theData = theData;
	controller.selectionStart = SelectionStart;
	controller.selectionEnd = SelectionEnd;
	controller.stereoMode = StereoMode ? YES : NO;
	
	return [controller runAsSheet];
}

#endif

@end

