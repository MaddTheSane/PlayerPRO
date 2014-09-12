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

@interface LengthWindowController : NSWindowController

@property (weak) IBOutlet LengthViewController *currentSize;
@property (weak, getter = getNewSize) IBOutlet LengthViewController *newSize;
@property (weak) IBOutlet NSBox *theCurrentBox;
@property (weak) IBOutlet NSBox *theNewBox;
@property (weak) IBOutlet NSMatrix *lengthCompensationMatrix;

@property NSRange selectionRange;
@property BOOL stereoMode;
@property (copy) PPPlugErrorBlock currentBlock;
@property (strong) PPSampleObject *theData;

- (IBAction)okay:(id)sender;
- (IBAction)cancel:(id)sender;

@end
