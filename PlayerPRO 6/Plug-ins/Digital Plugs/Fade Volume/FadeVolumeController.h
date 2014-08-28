//
//  FadeVolumeController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface FadeVolumeController : PPDigitalPluginWindowController
@property double fadeFrom;
@property double fadeTo;
@end
