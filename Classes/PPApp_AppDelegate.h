//
//  PPApp_AppDelegate.h
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import "PPSoundSettingsViewController.h"
@class PPLibrary;
@class PPPreferences;
@class PPMusicList;
@class PPInstrumentImporter;
@class PPInstrumentWindowController;
@class PPDigitalPlugInHandler;
@class PPCurrentlyPlayingIndex;
@class PPFilterPlugHandler;
@class PatternHandler;
@class PPInstrumentPlugHandler;

@interface PPApp_AppDelegate : NSDocumentController <NSApplicationDelegate, PPSoundSettingsViewControllerDelegate> {	
	NSMutableArray			*plugInInfos;
	IBOutlet NSMenu			*aboutPlugInMenu;
	
	IBOutlet NSMenu			*musicExportMenu;
	
	IBOutlet NSMenu			*newInstrumentMenu;
	IBOutlet NSMenu			*instrumentExportMenu;
	PPInstrumentWindowController *instrumentController;
	
	NSTimer					*timeChecker;
	
	PPPreferences			*preferences;
	PPInstrumentPlugHandler	*instrumentImporter;
	//PPDigitalPlugInHandler	*digitalHandler;
	//PPFilterPlugHandler		*filterHandler;
	
	MADDriverSettings		exportSettings;
	IBOutlet NSBox			*exportSettingsBox;
	PPSoundSettingsViewController *exportController;
	
	IBOutlet NSMenuItem *pauseDockMenuItem;
	
	NSUndoManager *undoManager;
	
	BOOL isQuitting;
}

@property (readonly) PPLibrary *madLib;

@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSWindow *exportWindow;

- (IBAction)toggleInfo:(id)sender;

- (IBAction)showInstrumentsList:(id)sender;
- (IBAction)exportInstrumentAs:(id)sender;
- (IBAction)deleteInstrument:(id)sender;
- (IBAction)saveInstrumentList:(id)sender;

- (IBAction)showPreferences:(id)sender;
- (IBAction)showBoxEditor:(id)sender;
- (IBAction)showClassicEditor:(id)sender;
- (IBAction)showDigitalEditor:(id)sender;

- (IBAction)openFile:(id)sender;
- (IBAction)saveMusic:(id)sender;
- (IBAction)saveMusicAs:(id)sender;
- (IBAction)newMusic:(id)sender;

- (IBAction)exportMusicAs:(id)sender;
- (IBAction)okayExportSettings:(id)sender;
- (IBAction)cancelExportSettings:(id)sender;

@end
