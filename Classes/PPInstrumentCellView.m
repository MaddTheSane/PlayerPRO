//
//  PPInstrumentCellView.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPInstrumentCellView.h"
#import "PPInstrumentWindowController.h"

@implementation PPInstrumentCellView
@synthesize numField;
@synthesize sampleButton;
@synthesize controller;

@synthesize isSample;
- (void)setIsSample:(BOOL)aisSample
{
	isSample = aisSample;
	if (isSample) {
		[numField setHidden:YES];
		[sampleButton setHidden:NO];
		//[loopImage setHidden:NO];
	} else {
		[numField setHidden:NO];
		[sampleButton setHidden:YES];
		[loopImage setHidden:YES];
	}
}

@synthesize isLoopingSample;
- (void)setIsLoopingSample:(BOOL)IsLoopingSample
{
	isLoopingSample = IsLoopingSample;
	if (isSample) {
		if (isLoopingSample) {
			[loopImage setHidden:NO];
		} else {
			[loopImage setHidden:YES];
		}		
	}
}

@synthesize isBlank;
- (void)setIsBlank:(BOOL)aisBlank
{
	isBlank = aisBlank;
	[self setNeedsDisplay:YES];
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    // Drawing code here.
	if (isBlank) {
		NSColor *grayOverlay = [NSColor colorWithDeviceWhite:0.65 alpha:1];
		[grayOverlay set];
		//[grayOverlay drawSwatchInRect:dirtyRect];
		[[NSBezierPath bezierPathWithRect:dirtyRect] fill];
	}
}

- (IBAction)playSample:(id)sender
{
	[controller playSample:sender];
}

@end
