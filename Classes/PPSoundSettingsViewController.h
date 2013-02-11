//
//  PPSoundSettingsViewController.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>

@protocol PPSoundSettingsViewControllerDelegate <NSObject>
@optional
- (void)soundOutDriverDidChange:(short)driv;
@required
- (void)soundOutBitsDidChange:(short)bits;
- (void)soundOutRateDidChange:(unsigned int)rat;
- (void)soundOutReverbDidChangeActive:(BOOL)isAct;
- (void)soundOutOversamplingDidChangeActive:(BOOL)isAct;
- (void)soundOutStereoDelayDidChangeActive:(BOOL)isAct;
- (void)soundOutSurroundDidChangeActive:(BOOL)isAct;
- (void)soundOutReverbStrengthDidChange:(short)rev;
- (void)soundOutReverbSizeDidChange:(short)rev;
- (void)soundOutOversamplingAmountDidChange:(short)ovs;
- (void)soundOutStereoDelayAmountDidChange:(short)std;

@end

@interface PPSoundSettingsViewController : NSViewController
{
	__arcweak id<PPSoundSettingsViewControllerDelegate> delegate;
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
	BOOL reverbActive;
	BOOL surroundActive;
	BOOL stereoDelayActive;
	BOOL oversamplingActive;
}

- (id)init;

- (void)settingsFromDriverSettings:(MADDriverSettings*)sett;

- (IBAction)changeBits:(id)sender;
- (IBAction)changeDriver:(id)sender;
- (IBAction)changeRate:(id)sender;
- (IBAction)changeChecked:(id)sender;
- (IBAction)changeOversampling:(id)sender;
- (IBAction)changeReverbAmount:(id)sender;
- (IBAction)changeReverbPercent:(id)sender;
- (IBAction)changeStereoDelay:(id)sender;

@property (readwrite, arcweak) id<PPSoundSettingsViewControllerDelegate> delegate;

@end
