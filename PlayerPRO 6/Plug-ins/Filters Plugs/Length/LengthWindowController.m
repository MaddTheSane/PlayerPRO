//
//  LengthWindowController.m
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import "LengthWindowController.h"
#import "LengthViewController.h"

@interface LengthWindowController ()

@end

static void *ConvertSampleSize(void *src, size_t srcSize, short amp, size_t dstSize, BOOL stereo)
{
#define LRVAL	3L
	
	short			*src16 = (short*)src, *dst16;
	char			*src8 = (char*)src, *dst8;
	long			realsrcSize, tempL = 0, tempR = 0;
	unsigned long	x, left, right, pos, newSize = dstSize;
	
	void *dst = calloc(newSize, 1);
	if (dst == NULL) return NULL;
	
	realsrcSize = srcSize;
	
	srcSize /= 100;
	dstSize /= 100;
	
	dst16 = (short*) dst;
	dst8 = (char*) dst;
	
	switch (amp) {
		case 8:
			for (x = 0; x < newSize; x++) {
				pos			= (x * srcSize << LRVAL) / dstSize;
				right		= pos & ((1 << LRVAL)-1);
				left		= (1 << LRVAL) - right;
				
				if (stereo) {
					pos >>= LRVAL;
					pos /= 2;
					pos *= 2;
					
					if (2 + pos >= realsrcSize) {
					} else {
						tempL = (left * src8[pos] + right * src8[2 + pos]) >> LRVAL;
					}
					
					dst8[x] = tempL;
					
					x++;
					
					if (3 + pos >= realsrcSize) {
					} else {
						tempR = (left * src8[1 + pos] + right * src8[3 + pos]) >> LRVAL;
					}
					
					dst8[x] = tempR;
				} else {
					pos >>= LRVAL;
					
					if (1 + pos >= realsrcSize)	{
					} else {
						tempL = (left * src8[pos] + right * src8[1 + pos]) >> LRVAL;
					}
					
					dst8[x] = tempL;
				}
			}
			break;
			
		case 16:
			for (x = 0; x < newSize/2; x++) {
				pos			= (x * srcSize << LRVAL) / dstSize;
				right		= pos & ((1 << LRVAL)-1);
				left		= (1 << LRVAL) - right;
				
				if (stereo) {
					pos >>= LRVAL;
					pos /= 2;
					pos *= 2;
					
					if (2 + pos >= realsrcSize/2) {
					} else {
						tempL = (left * src16[pos] + right * src16[2 + pos]) >> LRVAL;
					}
					
					dst16[x] = tempL;
					
					x++;
					
					if (3 + pos >= realsrcSize/2) {
					} else {
						tempR = (left * src16[1 + pos] + right * src16[3 + pos]) >> LRVAL;
					}
					
					dst16[x] = tempR;
					
					
				}
				else
				{
					pos >>= LRVAL;
					
					if (1 + pos >= realsrcSize/2) {
					} else {
						tempL = (left * src16[pos] + right * src16[1 + pos]) >> LRVAL;
					}
					
					dst16[x] = tempL;
				}
			}
			break;
	}
	
	return dst;
}


@implementation LengthWindowController
@synthesize currentSize;
@synthesize newSize;
@synthesize theCurrentBox = currentBox;
@synthesize theNewBox = newBox;
@synthesize lengthCompensationMatrix;

- (instancetype)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) {
		// Initialization code here.
	}
	
	return self;
}

- (IBAction)okay:(id)sender
{
	[(NSApplication*)NSApp endSheet:self.window];
	_currentBlock(MADNoErr);
}

- (IBAction)cancel:(id)sender
{
	[(NSApplication*)NSApp endSheet:self.window];
	_currentBlock(MADUserCanceledErr);
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	self.theCurrentBox.contentView = self.currentSize.view;
	self.theNewBox.contentView = self.newSize.view;
	currentSize.isNewSize = NO;
	newSize.isNewSize = YES;
}

@end
