//
//  SamplingRateWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
@import PlayerPROKit;

@interface SamplingRateWindowController : NSWindowController
@property (strong) PPSampleObject *sampleToEdit;
@property unsigned int currentRate;
@property unsigned int changedRate;
@end
