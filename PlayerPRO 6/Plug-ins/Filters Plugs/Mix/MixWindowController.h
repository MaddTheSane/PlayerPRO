//
//  MixWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface MixWindowController : NSWindowController
@property double mixOriginal;
@property double mixPasteboard;
@property (weak) IBOutlet NSTextField *instrumentLabel;
@property (weak) IBOutlet NSTextField *pasteboardLabel;
@property NSRange selectionRange;
@property BOOL stereoMode;
@property (copy) PPPlugErrorBlock currentBlock;
@property (strong) PPSampleObject *theData;

- (IBAction)okay:(id)sender;
- (IBAction)cancel:(id)sender;
@end
