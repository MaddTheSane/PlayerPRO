//
//  LengthWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>

@class LengthViewController;

@interface LengthWindowController : NSWindowController

@property (weak) IBOutlet LengthViewController *currentSize;
@property (weak) IBOutlet LengthViewController *newSize;

@end
