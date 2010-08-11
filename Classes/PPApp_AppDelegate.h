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

@interface PPApp_AppDelegate : NSObject {
    IBOutlet NSWindow *window;
    IBOutlet NSDrawer *infoDrawer;
    IBOutlet NSToolbar *toolbar;
	
	IBOutlet NSTextField *fileName;
	IBOutlet NSTextField *internalName;
	IBOutlet NSTextField *fileSize;
	IBOutlet NSTextField *musicInstrument;
	IBOutlet NSTextField *musicPatterns;
	IBOutlet NSTextField *musicPlugType;
	IBOutlet NSTextField *fileLocation;
	
	PPPreferences	*preferences;
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
- (BOOL)loadMADMusic:(MADMusic*)musicToLoad;


@property (assign) NSWindow *window;


@end
