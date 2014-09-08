//
//  NoteTranslateController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface NoteTranslateController : NSWindowController
@property NSInteger transAmount;
@property Pcmd *thePcmd;
@property (strong) PPPlugErrorBlock currentBlock;

- (IBAction)okay:(id)sender;
- (IBAction)cancel:(id)sender;
@end
