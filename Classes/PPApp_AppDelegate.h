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
    NSWindow		*window;
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


@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet PPPreferences *preferences;


@end
