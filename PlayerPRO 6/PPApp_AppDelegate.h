//
//  PPApp_AppDelegate.h
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import "PPExportObject.h"

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

@interface PPApp_AppDelegate : NSDocumentController <NSApplicationDelegate, PPExportObjectDelegate>
{
	NSMutableArray			*plugInInfos;
	
	PPPreferences			*preferences;
	PPInstrumentPlugHandler	*instrumentImporter;
	PPDigitalPlugHandler	*digitalHandler;
	PPFilterPlugHandler		*filterHandler;
}
@property (strong, readonly) NSMutableArray *thePPColors;
@property (strong, readonly) NSDictionary *trackerDict;
@property (strong, readonly) NSArray *trackerUTIs;
@property (strong, readonly) PPInstrumentPlugHandler *instrumentPlugHandler;
@property (strong, readonly) PPDigitalPlugHandler *digitalHandler;
@property (strong, readonly) PPFilterPlugHandler *filterHandler;

@property (weak) IBOutlet NSMenu *aboutPlugInMenu;
@property (weak) IBOutlet NSMenu *musicExportMenu;
@property (weak, getter = getNewInstrumentMenu) IBOutlet NSMenu *newInstrumentMenu;
@property (weak) IBOutlet NSMenu *instrumentExportMenu;

@property (weak) IBOutlet NSPanel *window;

- (IBAction)showPreferences:(id)sender;
- (IBAction)openFile:(id)sender;

- (void)addExportObject:(PPExportObject *)expObj;

@end
