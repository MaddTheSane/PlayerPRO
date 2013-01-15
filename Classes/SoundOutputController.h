//
//  SoundOutputController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface SoundOutputController : NSViewController {
    IBOutlet __weak NSButton *oversampling;
    IBOutlet __weak NSButton *reverb;
    IBOutlet __weak NSButton *stereoDelay;
    IBOutlet __weak NSButton *surround;
    IBOutlet __weak NSMatrix *rate;
    IBOutlet __weak NSMatrix *outputBits;
    IBOutlet __weak NSMatrix *soundDriver;
    IBOutlet __weak NSPopUpButton *oversamplingNum;
    IBOutlet __weak NSPopUpButton *reverbNum;
    IBOutlet __weak NSPopUpButton *stereoDelayNum;
    IBOutlet __weak NSPopUpButton *reverbPercent;
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
