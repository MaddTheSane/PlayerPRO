//
//  PPApp_AppDelegate.h
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>
@class PPLibrary;
@class PPPreferences;
@class PPMusicList;
@class PPInstrumentWindowController;
@class PPDigitalPlugHandler;
@class PPCurrentlyPlayingIndex;
@class PPFilterPlugHandler;
@class PatternHandler;
@class PPInstrumentPlugHandler;
@class PPExportObject;

@interface PPApp_AppDelegate : NSDocumentController <NSApplicationDelegate>
{
	NSMutableArray			*plugInInfos;
	IBOutlet NSMenu			*aboutPlugInMenu;
	
	IBOutlet NSMenu			*musicExportMenu;
	
	IBOutlet NSMenu			*newInstrumentMenu;
	IBOutlet NSMenu			*instrumentExportMenu;
	PPInstrumentWindowController *instrumentController;
	
	PPPreferences			*preferences;
	PPInstrumentPlugHandler	*instrumentImporter;
	PPDigitalPlugHandler	*digitalHandler;
	PPFilterPlugHandler		*filterHandler;
	
	BOOL isQuitting;
}

@property (readonly) PPLibrary *madLib;
@property (weak) IBOutlet NSWindow *window;
- (IBAction)showPreferences:(id)sender;
- (IBAction)openFile:(id)sender;

//- (IBAction)toggleInfo:(id)sender;

- (IBAction)showBoxEditor:(id)sender;
- (IBAction)showClassicEditor:(id)sender;
- (IBAction)showDigitalEditor:(id)sender;

- (void)addExportObject:(PPExportObject *)expObj;

@end
