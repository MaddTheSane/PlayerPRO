//
//  AppDelegate.h
//  PlayerPRO Player
//
//  Created by C.W. Betts on 9/6/13.
//
//

#import <Cocoa/Cocoa.h>
#import "PPSoundSettingsViewController.h"
#import "PPPreferences.h"
#import "PPMusicList.h"
#include <PlayerPROCore/PlayerPROCore.h>
@class PPInstrumentWindowController;
@class PPCurrentlyPlayingIndex;

@interface AppDelegate : NSObject <NSApplicationDelegate, PPSoundSettingsViewControllerDelegate, NSTableViewDelegate, NSToolbarDelegate>
{
	IBOutlet NSButton		*loopButton;
    IBOutlet NSButton		*playButton;
    IBOutlet NSButton		*stopButton;
	IBOutlet NSTextField	*songCurTime;
	IBOutlet NSTextField	*songTotalTime;
	IBOutlet NSSlider		*songPos;
	IBOutlet NSButton		*pauseButton;
	
	IBOutlet NSMenu			*aboutPlugInMenu;
	
	IBOutlet NSMenu			*musicExportMenu;
	
	IBOutlet NSDrawer		*infoDrawer;
	IBOutlet NSTextField	*fileName;
	IBOutlet NSTextField	*internalName;
	IBOutlet NSTextField	*fileSize;
	IBOutlet NSTextField	*musicInstrument;
	IBOutlet NSTextField	*musicPatterns;
	IBOutlet NSTextField	*musicPlugType;
	IBOutlet NSTextField	*musicSignature;
	IBOutlet NSTextField	*fileLocation;
	
	IBOutlet NSTableView	*tableView;
	IBOutlet NSArrayController *musicListController;
	NSTimer					*timeChecker;
	MADDriverRec			*madDriver;
	MADMusic				*music;
	MADLibrary				*madLib;
	
	MADDriverSettings		exportSettings;
	IBOutlet NSBox			*exportSettingsBox;
	PPSoundSettingsViewController *exportController;
}

@property (strong) NSString* musicName;

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSWindow *exportWindow;
@property (assign) IBOutlet NSMenuItem *pauseDockMenuItem;

@property (assign) IBOutlet NSPanel *toolsPanel;
@property (readonly, strong) PPInstrumentWindowController *instrumentController;
@property (getter=isPaused, readwrite) BOOL paused;

- (IBAction)showMusicList:(id)sender;
- (IBAction)sortMusicList:(id)sender;
- (IBAction)playSelectedMusic:(id)sender;
- (IBAction)addMusic:(id)sender;
- (IBAction)removeSelectedMusic:(id)sender;
- (IBAction)clearMusicList:(id)sender;
- (IBAction)saveMusicList:(id)sender;

- (IBAction)toggleInfo:(id)sender;

- (IBAction)showInstrumentsList:(id)sender;

- (IBAction)showPreferences:(id)sender;

- (void)soundPreferencesDidChange:(NSNotification *)notification;

- (BOOL)loadMusicURL:(NSURL*)musicToLoad;
- (BOOL)loadMusicURL:(NSURL*)musicToLoad error:(out NSError *__autoreleasing*)theErr;

- (IBAction)openFile:(id)sender;
- (IBAction)saveMusic:(id)sender;
- (IBAction)saveMusicAs:(id)sender;

- (IBAction)exportMusicAs:(id)sender;
- (IBAction)okayExportSettings:(id)sender;
- (IBAction)cancelExportSettings:(id)sender;

- (IBAction)showTools:(id)sender;
- (IBAction)fastForwardButtonPressed:(id)sender;
- (IBAction)loopButtonPressed:(id)sender;
- (IBAction)nextButtonPressed:(id)sender;
- (IBAction)playButtonPressed:(id)sender;
- (IBAction)prevButtonPressed:(id)sender;
- (IBAction)rewindButtonPressed:(id)sender;
- (IBAction)sliderChanged:(id)sender;
- (IBAction)stopButtonPressed:(id)sender;
- (IBAction)pauseButtonPressed:(id)sender;

@end
