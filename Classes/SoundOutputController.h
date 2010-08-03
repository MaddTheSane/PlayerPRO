//
//  SoundOutputController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface SoundOutputController : NSViewController {
    IBOutlet NSButton *oversampling;
    IBOutlet NSButton *reverb;
    IBOutlet NSButton *stereoDelay;
    IBOutlet NSButton *surround;
}

@end
