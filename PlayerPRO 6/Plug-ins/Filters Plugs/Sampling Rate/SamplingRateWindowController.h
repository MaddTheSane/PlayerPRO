//
//  SamplingRateWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface SamplingRateWindowController : NSWindowController
@property unsigned int currentRate;
@property unsigned int changedRate;
@property NSRange selectionRange;
@property BOOL stereoMode;
@property (copy) PPPlugErrorBlock currentBlock;
@property (strong) PPSampleObject *theData;

@property (strong) NSWindow *parentWindow;

- (IBAction)okay:(id)sender;
- (IBAction)cancel:(id)sender;

@end
