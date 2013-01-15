//
//  PPApp_AppDelegate.h
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>
@class PPPreferences;
@class PPMusicList;
@class PPInstrumentImporter;
@class PPInstrumentWindowController;
@class PPDigitalPlugInHandler;
@class PPCurrentlyPlayingIndex;

@interface PPApp_AppDelegate : NSObject <NSApplicationDelegate, NSTableViewDelegate> {
	IBOutlet __weak NSPanel		*toolsPanel;
	IBOutlet __weak NSButton		*loopButton;
    IBOutlet __weak NSButton		*playButton;
    IBOutlet __weak NSButton		*recordButton;
    IBOutlet __weak NSTextField	*songLabel;
    IBOutlet __weak NSButton		*stopButton;
	IBOutlet __weak NSTextField	*songCurTime;
	IBOutlet __weak NSTextField	*songTotalTime;
	IBOutlet __weak NSSlider		*songPos;
	IBOutlet __weak NSButton		*pauseButton;
	BOOL					paused;
	
	NSMutableArray			*plugInInfos;
	IBOutlet __weak NSMenu			*aboutPlugInMenu;
	
	IBOutlet __weak NSMenu			*musicExportMenu;
	
	IBOutlet __weak NSMenu			*instrumentExportMenu;
	PPInstrumentWindowController *instrumentController;
	
	IBOutlet  __weak NSWindow *window;
	
	IBOutlet __weak NSTextField	*fileName;
	IBOutlet __weak NSTextField	*internalName;
	IBOutlet __weak NSTextField	*fileSize;
	IBOutlet __weak NSTextField	*musicInstrument;
	IBOutlet __weak NSTextField	*musicPatterns;
	IBOutlet __weak NSTextField	*musicPlugType;
	IBOutlet __weak NSTextField	*musicSignature;
	IBOutlet __weak NSTextField	*fileLocation;
	
	IBOutlet __weak NSTableView	*tableView;
	IBOutlet __weak NSArrayController *musicListController;
	NSTimer					*timeChecker;
	MADDriverRec			*MADDriver;
	MADMusic				*Music;
	MADLibrary				*MADLib;	
	
	PPMusicList				*musicList;
	PPCurrentlyPlayingIndex	*currentlyPlayingIndex;
	PPCurrentlyPlayingIndex	*previouslyPlayingIndex;
	PPPreferences			*preferences;
	PPInstrumentImporter	*instrumentImporter;
	PPDigitalPlugInHandler	*digitalHandler;
}

@property (getter=isPaused, readwrite) BOOL paused;

@property (weak) NSWindow *window;
- (IBAction)showMusicList:(id)sender;
- (IBAction)sortMusicList:(id)sender;
- (IBAction)playSelectedMusic:(id)sender;
- (IBAction)addMusic:(id)sender;
- (IBAction)removeSelectedMusic:(id)sender;
- (IBAction)clearMusicList:(id)sender;
- (IBAction)saveMusicList:(id)sender;

- (IBAction)showInstrumentsList:(id)sender;
- (IBAction)exportInstrumentAs:(id)sender;
- (IBAction)deleteInstrument:(id)sender;

- (IBAction)showPreferences:(id)sender;
- (IBAction)showBoxEditor:(id)sender;
- (IBAction)showClassicEditor:(id)sender;
- (IBAction)showDigitalEditor:(id)sender;

- (void)preferencesDidChange:(NSNotification *)notification;
- (void)soundPreferencesDidChange:(NSNotification *)notification;

- (BOOL)loadMusicURL:(NSURL*)musicToLoad;

- (IBAction)openFile:(id)sender;
- (IBAction)saveMusic:(id)sender;
- (IBAction)saveMusicAs:(id)sender;
- (IBAction)exportMusicAs:(id)sender;

- (IBAction)showTools:(id)sender;
- (IBAction)fastForwardButtonPressed:(id)sender;
- (IBAction)loopButtonPressed:(id)sender;
- (IBAction)nextButtonPressed:(id)sender;
- (IBAction)playButtonPressed:(id)sender;
- (IBAction)prevButtonPressed:(id)sender;
- (IBAction)recordButtonPressed:(id)sender;
- (IBAction)rewindButtonPressed:(id)sender;
- (IBAction)sliderChanged:(id)sender;
- (IBAction)stopButtonPressed:(id)sender;
- (IBAction)pauseButtonPressed:(id)sender;

@end
