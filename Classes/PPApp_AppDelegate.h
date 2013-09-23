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
	
	PPPreferences			*preferences;
	PPInstrumentPlugHandler	*instrumentImporter;
	//PPDigitalPlugInHandler	*digitalHandler;
	//PPFilterPlugHandler		*filterHandler;
	
	MADDriverSettings		exportSettings;
	IBOutlet NSBox			*exportSettingsBox;
	PPSoundSettingsViewController *exportController;
	
	BOOL isQuitting;
}

@property (readonly) PPLibrary *madLib;
@property (weak) IBOutlet NSWindow *window;

//- (IBAction)toggleInfo:(id)sender;


- (IBAction)showPreferences:(id)sender;
- (IBAction)showBoxEditor:(id)sender;
- (IBAction)showClassicEditor:(id)sender;
- (IBAction)showDigitalEditor:(id)sender;

- (IBAction)openFile:(id)sender;

@end
