//
//  PPSmallPianoView.m
//  PPMacho
//
//  Created by C.W. Betts on 3/16/14.
//
//

#import "PPSmallPianoView.h"

@implementation PPSmallPianoView

- (instancetype)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];
	if (self) {
		// Initialization code here.
	}
	return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
	[super drawRect:dirtyRect];
	NSRect tmpRect = self.frame;
	tmpRect.origin.x += 1;
	tmpRect.origin.y += 1;
	tmpRect.size.height -= 2;
	tmpRect.size.width -= 2;
	[[NSColor blackColor] setStroke];
	[NSBezierPath setDefaultLineWidth:1];
	[NSBezierPath strokeRect:tmpRect];
	// Drawing code here.
}

@end
