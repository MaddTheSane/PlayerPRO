//
//  PPInstrumentCellView.m
//  PPMacho
//
//  Created by C.W. Betts on 1/10/13.
//
//

#import "PPInstrumentCellView.h"

@implementation PPInstrumentCellView

@synthesize numField;
@synthesize isSample;
@synthesize sampleButton;

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)setIsSample:(BOOL)aisSample
{
	isSample = aisSample;
}

- (void)drawRect:(NSRect)dirtyRect
{
    // Drawing code here.
}

@end
