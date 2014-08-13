//
//  PPDocument.h
//  PPMacho
//
//  Created by C.W. Betts on 9/7/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>
#import "PPSoundSettingsViewController.h"

@class BoxViewController;
@class DigitalViewController;
@class ClassicalViewController;
@class WaveViewController;

@interface PPDocument : NSDocument <PPSoundSettingsViewControllerDelegate>
@property (weak) IBOutlet NSWindow	*exportWindow;
@property (weak) IBOutlet NSBox		*exportSettingsBox;
@property (weak) IBOutlet NSButton *fastForwardButton;
@property (weak) IBOutlet NSButton *playButton;
@property (weak) IBOutlet NSButton *reverseButton;
@property (weak) IBOutlet NSTextField *currentTimeLabel;
@property (weak) IBOutlet NSTextField *totalTimeLabel;
@property (weak) IBOutlet NSSlider *playbackPositionSlider;
@property (weak) IBOutlet NSTabView *editorsTab;

@property (weak) IBOutlet BoxViewController *boxController;
@property (weak) IBOutlet DigitalViewController *digitalController;
@property (weak) IBOutlet ClassicalViewController *classicalController;
@property (weak) IBOutlet WaveViewController *waveController;

@property (strong) PPSoundSettingsViewController *exportController;
@property (readonly, strong) PPDriver *theDriver;
@property (copy) NSString *musicName;
@property (copy) NSString *musicInfo;
@property (copy) NSString *authorString;
- (PPMusicObject*)wrapper;

- (void)importMusicObject:(PPMusicObject*)theObj;
- (IBAction)exportMusicAs:(id)sender;

- (IBAction)showBoxEditor:(id)sender;
- (IBAction)showClassicEditor:(id)sender;
- (IBAction)showDigitalEditor:(id)sender;
- (IBAction)showWavePreview:(id)sender;

@end
