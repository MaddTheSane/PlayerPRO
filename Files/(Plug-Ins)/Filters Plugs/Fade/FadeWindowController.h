//
//  FadeWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>

@interface FadeWindowController : NSWindowController
@property double fadeFrom;
@property double fadeTo;
- (IBAction)okOrCancel:(id)sender;

@end
