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
    IBOutlet NSMatrix *rate;
    IBOutlet NSMatrix *outputBits;
    IBOutlet NSMatrix *soundDriver;
    IBOutlet NSPopUpButton *oversamplingNum;
    IBOutlet NSPopUpButton *reverbNum;
    IBOutlet NSPopUpButton *stereoDelayNum;
    IBOutlet NSPopUpButton *reverbPercent;
}
- (IBAction)changeBits:(id)sender;
- (IBAction)changeDriver:(id)sender;
- (IBAction)changeRate:(id)sender;
- (IBAction)changeChecked:(id)sender;
- (IBAction)changeOversampling:(id)sender;
- (IBAction)changeReverbAmount:(id)sender;
- (IBAction)changeReverbPercent:(id)sender;
- (IBAction)changeStereoDelay:(id)sender;
@end
