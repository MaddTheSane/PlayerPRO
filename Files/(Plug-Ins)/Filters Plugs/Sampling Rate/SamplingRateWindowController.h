//
//  SamplingRateWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>

@interface SamplingRateWindowController : NSWindowController
@property unsigned int currentRate;
@property unsigned int changedRate;
- (IBAction)okOrCancel:(id)sender;
@end
