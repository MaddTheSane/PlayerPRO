//
//  PPSoundSettingsViewController.h
//  PPMacho
//
//  Created by C.W. Betts on 1/17/13.
//
//

#include <PlayerPROCore/PlayerPROCore.h>
#import <Cocoa/Cocoa.h>

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@protocol PPSoundSettingsViewControllerDelegate <NSObject>
- (void)soundOutBitsDidChange:(short)bits;
- (void)soundOutRateDidChange:(unsigned int)rat;
- (void)soundOutReverbDidChangeActive:(BOOL)isAct;
- (void)soundOutOversamplingDidChangeActive:(BOOL)isAct;
- (void)soundOutStereoDelayDidChangeActive:(BOOL)isAct;
- (void)soundOutSurroundDidChangeActive:(BOOL)isAct;
- (void)soundOutReverbStrengthDidChange:(int)rev;
- (void)soundOutReverbSizeDidChange:(int)rev;
- (void)soundOutOversamplingAmountDidChange:(int)ovs;
- (void)soundOutStereoDelayAmountDidChange:(int)std;
@optional
- (void)soundOutDriverDidChange:(short)driv;
@end

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

+ (instancetype)newSoundSettingWindow NS_RETURNS_RETAINED;
- (instancetype)init;
- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil NS_DESIGNATED_INITIALIZER;
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
