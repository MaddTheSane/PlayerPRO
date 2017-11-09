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
@synthesize theData;

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

- (IBAction)okay:(id)sender
{
	int newFreq = self.changedRate;
	NSData *newPtr = [self convertSampleC4SPD:self.theData newRate:newFreq];
	
	if (newPtr != NULL) {
		theData.data	= newPtr;
		theData.c2spd	= newFreq;
		theData.loopBegin = (theData.loopBegin * (newFreq / 100)) / (int) (theData.c2spd / 100);
		theData.loopSize = (theData.loopSize * (newFreq / 100)) / (int) (theData.c2spd / 100);
	} else {
		[_parentWindow endSheet:self.window];
		_currentBlock([NSError errorWithDomain:PPMADErrorDomain code:MADUnknownErr userInfo:nil]);
		return;
	}
	
	if (theData.loopBegin < 0) {
		theData.loopSize += theData.loopBegin;
		theData.loopBegin = 0;
	}
	if (theData.loopBegin > theData.data.length) {
		theData.loopBegin = 0;
		theData.loopSize = 0;
	}
	if (theData.loopSize < 0)
		theData.loopSize = 0;
	if (theData.loopBegin + theData.loopSize > theData.data.length)
		theData.loopSize = (int)(theData.data.length - theData.loopBegin);

	
	[_parentWindow endSheet:self.window];
	_currentBlock(nil);
}

- (IBAction)cancel:(id)sender
{
	[_parentWindow endSheet:self.window];
	_currentBlock([NSError errorWithDomain:PPMADErrorDomain code:MADUserCancelledErr userInfo:nil]);
}

- (instancetype)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) {
		
	}
	
	return self;
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

@end

