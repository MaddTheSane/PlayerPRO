//
//  LengthWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@class LengthViewController;

@interface LengthWindowController : PPFilterPluginWindowController

@property (weak) IBOutlet LengthViewController *currentSize;
@property (weak, getter = getNewSize) IBOutlet LengthViewController *newSize;

@end
