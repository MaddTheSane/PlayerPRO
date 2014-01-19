//
//  PPHilightableButton.m
//  PPMacho
//
//  Created by C.W. Betts on 12/21/12.
//
//

#import "PPHilightableButton.h"

@implementation PPHilightableButton

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)setState:(NSInteger)value
{
	[super setState:value];
	
	[self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
	if ([self state] == NSOnState) {
		NSColor *grayOverlay = [NSColor colorWithDeviceWhite:0.2 alpha:0.5];
		[grayOverlay set];
		//[grayOverlay drawSwatchInRect:dirtyRect];
		[[NSBezierPath bezierPathWithRect:dirtyRect] fill];
		
	}
}

@end
