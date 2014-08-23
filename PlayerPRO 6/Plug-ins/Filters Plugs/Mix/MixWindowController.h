//
//  MixWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface MixWindowController : PPFilterPluginWindowController
@property double mixOriginal;
@property double mixPasteboard;
@property (weak) IBOutlet NSTextField *instrumentLabel;
@property (weak) IBOutlet NSTextField *pasteboardLabel;
@end
