//
//  FadeVolumeController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface FadeVolumeController : NSWindowController
@property double fadeFrom;
@property double fadeTo;
@property Pcmd *thePcmd;
@property (copy) PPPlugErrorBlock currentBlock;

@property (strong) NSWindow *parentWindow;
@end
