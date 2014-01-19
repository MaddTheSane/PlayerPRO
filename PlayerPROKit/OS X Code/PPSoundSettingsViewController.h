//
//  PPSoundSettingsViewController.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#include <PlayerPROCore/PlayerPROCore.h>
#import <Foundation/NSObject.h>
#import <AppKit/NSViewController.h>

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

@class NSButton, NSMatrix, NSPopUpButton;

@interface PPSoundSettingsViewController : NSViewController
@property (weak) IBOutlet NSButton *oversampling;
@property (weak) IBOutlet NSButton *reverb;
@property (weak) IBOutlet NSButton *stereoDelay;
@property (weak) IBOutlet NSButton *surround;
@property (weak) IBOutlet NSMatrix *rate;
@property (weak) IBOutlet NSMatrix *outputBits;
@property (weak) IBOutlet NSMatrix *soundDriver;
@property (weak) IBOutlet NSPopUpButton *oversamplingNum;
@property (weak) IBOutlet NSPopUpButton *reverbNum;
@property (weak) IBOutlet NSPopUpButton *stereoDelayNum;
@property (weak) IBOutlet NSPopUpButton *reverbPercent;
@property (weak) IBOutlet id<PPSoundSettingsViewControllerDelegate> delegate;

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
@end
