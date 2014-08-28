//
//  SamplingRateWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface SamplingRateWindowController : PPFilterPluginWindowController
@property unsigned int currentRate;
@property unsigned int changedRate;
@end
