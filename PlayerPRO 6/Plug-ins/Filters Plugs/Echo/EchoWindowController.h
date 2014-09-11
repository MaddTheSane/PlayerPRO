//
//  EchoWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface EchoWindowController : NSWindowController
@property double echoStrength;
@property int echoDelay;
@property NSRange selectionRange;
@property (strong) PPSampleObject *theData;
@property (copy) PPPlugErrorBlock currentBlock;
@end
