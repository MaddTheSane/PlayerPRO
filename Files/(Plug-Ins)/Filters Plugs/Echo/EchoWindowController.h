//
//  EchoWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>

@interface EchoWindowController : NSWindowController
@property double echoStrength;
@property int echoDelay;
- (IBAction)okOrCancel:(id)sender;
@end
