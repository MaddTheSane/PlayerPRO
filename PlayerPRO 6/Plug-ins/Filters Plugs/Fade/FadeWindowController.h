//
//  FadeWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface FadeWindowController : NSWindowController
@property double fadeFrom;
@property double fadeTo;
@property BOOL stereoMode;
@property (strong) PPSampleObject *theData;
@property (copy) PPPlugErrorBlock currentBlock;
@end
