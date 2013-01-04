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

@interface PPApp_AppDelegate : NSObject <NSApplicationDelegate, NSTableViewDelegate> {
	IBOutlet NSPanel *toolsPanel;
	IBOutlet NSButton *loopButton;
    IBOutlet NSButton *playButton;
    IBOutlet NSButton *recordButton;
    IBOutlet NSTextField *songLabel;
    IBOutlet NSButton *stopButton;
	IBOutlet NSTextField *songCurTime;
	IBOutlet NSTextField *songTotalTime;
	IBOutlet NSSlider *songPos;
	IBOutlet NSButton *pauseButton;
	BOOL paused;
	
	NSArray *plugInInfos;
	IBOutlet NSMenu *aboutPlugInMenu;
	
	IBOutlet NSMenu *instrumentExportMenu;
	PPInstrumentWindowController *instrumentController;
	
	IBOutlet  __unsafe_unretained NSWindow *window;
	
	IBOutlet NSTextField *fileName;
	IBOutlet NSTextField *internalName;
	IBOutlet NSTextField *fileSize;
	IBOutlet NSTextField *musicInstrument;
	IBOutlet NSTextField *musicPatterns;
	IBOutlet NSTextField *musicPlugType;
	IBOutlet NSTextField *musicSignature;
	IBOutlet NSTextField *fileLocation;
	
	IBOutlet NSTableView *tableView;
	IBOutlet NSArrayController *musicListController;
	NSTimer *timeChecker;
	MADDriverRec	*MADDriver;
	MADMusic		*Music;
	MADLibrary		*MADLib;	
	
	PPMusicList		*musicList;
	NSInteger		currentlyPlayingIndex;
	PPPreferences	*preferences;
	PPInstrumentImporter *instrumentImporter;
	PPDigitalPlugInHandler *digitalHandler;
}

@property (getter=isPaused, readwrite) BOOL paused;

@property (assign) NSWindow *window;
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
- (void)digitalEditorPreferencesDidChange:(NSNotification *)notification;

- (BOOL)loadMusicURL:(NSURL*)musicToLoad;

- (IBAction)openFile:(id)sender;


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
