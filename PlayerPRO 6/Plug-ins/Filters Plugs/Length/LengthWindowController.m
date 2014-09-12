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
