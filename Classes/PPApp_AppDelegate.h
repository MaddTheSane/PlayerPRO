//
//  PPApp_AppDelegate.h
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "PlayerPROCore/PlayerPROCore.h"
@class PPPreferences;
@class PPMusicList;

@interface PPApp_AppDelegate : NSObject {
    IBOutlet NSWindow *window;
	
	IBOutlet NSTextField *fileName;
	IBOutlet NSTextField *internalName;
	IBOutlet NSTextField *fileSize;
	IBOutlet NSTextField *musicInstrument;
	IBOutlet NSTextField *musicPatterns;
	IBOutlet NSTextField *musicPlugType;
	IBOutlet NSTextField *musicSignature;
	IBOutlet NSTextField *fileLocation;
    IBOutlet NSTableColumn *musicTable;
	
	PPPreferences	*preferences;
	PPMusicList		*musicList;
	MADDriverRec	*MADDriver;
	MADMusic		*Music;
	MADLibrary		*MADLib;	
}
- (IBAction)showMusicList:(id)sender;
- (IBAction)exportInstrumentAs:(id)sender;
- (IBAction)showInstrumentsList:(id)sender;
- (IBAction)showTools:(id)sender;
- (IBAction)showPreferences:(id)sender;
- (IBAction)deleteInstrument:(id)sender;
- (IBAction)showBoxEditor:(id)sender;
- (IBAction)showClassicEditor:(id)sender;
- (IBAction)showDigitalEditor:(id)sender;
- (IBAction)getMusicInfo:(id)sender;
- (BOOL)loadMusicFile:(NSURL*)musicToLoad;

-(void)preferencesDidChange:(NSNotification *)notification;
-(void)soundPreferencesDidChange:(NSNotification *)notification;


@property (assign) NSWindow *window;


@end
