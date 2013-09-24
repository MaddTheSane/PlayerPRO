//
//  EchoWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface EchoWindowController : PPFilterPluginWindowController
@property double echoStrength;
@property int echoDelay;
@end
