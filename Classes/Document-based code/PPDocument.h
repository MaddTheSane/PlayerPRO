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

@interface PPDocument : NSDocument <PPSoundSettingsViewControllerDelegate>
@property (weak) IBOutlet NSWindow	*exportWindow;
@property (weak) IBOutlet NSBox		*exportSettingsBox;
@property (weak) IBOutlet NSButton *fastForwardButton;
@property (weak) IBOutlet NSButton *playButton;
@property (weak) IBOutlet NSButton *reverseButton;
@property (weak) IBOutlet NSTextField *currentTimeLabel;
@property (weak) IBOutlet NSTextField *totalTimeLabel;
@property (weak) IBOutlet NSSlider *playbackPositionSlider;
@property (weak) IBOutlet BoxViewController *boxController;
@property (weak) IBOutlet NSTabView *editorsTab;

@property (strong) PPSoundSettingsViewController *exportController;
@property (copy) NSString *musicName;
@property (copy) NSString *musicInfo;
@property (copy) NSString *authorString;

- (void)importMusicObject:(PPMusicObject*)theObj;
- (void)importMusicObjectWrapper:(PPMusicObjectWrapper*)theWrap;
- (IBAction)exportMusicAs:(id)sender;

- (IBAction)showBoxEditor:(id)sender;
- (IBAction)showClassicEditor:(id)sender;
- (IBAction)showDigitalEditor:(id)sender;
- (IBAction)showWavePreview:(id)sender;

@end
