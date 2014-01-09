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
@class PPDigitalPlugHandler;
@class PPCurrentlyPlayingIndex;
@class PPFilterPlugHandler;
@class PatternHandler;
@class PPInstrumentPlugHandler;
@class PPExportObject;

extern __weak PPLibrary *globalMadLib;

@interface PPApp_AppDelegate : NSDocumentController <NSApplicationDelegate>
{
	NSMutableArray			*plugInInfos;
	IBOutlet NSMenu			*aboutPlugInMenu;
	
	IBOutlet NSMenu			*musicExportMenu;
	
	IBOutlet NSMenu			*newInstrumentMenu;
	IBOutlet NSMenu			*instrumentExportMenu;
	
	PPPreferences			*preferences;
	PPInstrumentPlugHandler	*instrumentImporter;
	PPDigitalPlugHandler	*digitalHandler;
	PPFilterPlugHandler		*filterHandler;
	
	BOOL isQuitting;
}

@property (weak) IBOutlet NSWindow *window;
- (IBAction)showPreferences:(id)sender;
- (IBAction)openFile:(id)sender;

- (void)addExportObject:(PPExportObject *)expObj;

@end
