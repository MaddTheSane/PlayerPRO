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

@interface PPApp_AppDelegate : NSObject <NSApplicationDelegate> {
	IBOutlet NSButton *loopButton;
    IBOutlet NSButton *playButton;
    IBOutlet NSButton *recordButton;
    IBOutlet NSTextField *songLabel;
    IBOutlet NSButton *stopButton;
	IBOutlet NSTextField *songCurTime;
	IBOutlet NSTextField *songTotalTime;
	IBOutlet NSSlider *songPos;
	
	IBOutlet NSTextField *fileName;
	IBOutlet NSTextField *internalName;
	IBOutlet NSTextField *fileSize;
	IBOutlet NSTextField *musicInstrument;
	IBOutlet NSTextField *musicPatterns;
	IBOutlet NSTextField *musicPlugType;
	IBOutlet NSTextField *musicSignature;
	IBOutlet NSTextField *fileLocation;
	
	MADDriverRec	*MADDriver;
	MADMusic		*Music;
	MADLibrary		*MADLib;	
	
    IBOutlet NSWindow *window;
	
	IBOutlet NSTableView *tableView;
    
	
	PPMusicList		*musicList;
	PPPreferences	*preferences;
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

-(void)preferencesDidChange:(NSNotification *)notification;


@property (assign) NSWindow *window;


- (IBAction)fastForwardButtonPressed:(id)sender;
- (IBAction)loopButtonPressed:(id)sender;
- (IBAction)nextButtonPressed:(id)sender;
- (IBAction)playButtonPressed:(id)sender;
- (IBAction)prevButtonPressed:(id)sender;
- (IBAction)recordButtonPressed:(id)sender;
- (IBAction)rewindButtonPressed:(id)sender;
- (IBAction)sliderChanged:(id)sender;
- (IBAction)stopButtonPressed:(id)sender;

- (void)soundPreferencesDidChange:(NSNotification *)notification;

- (void)digitalEditorPreferencesDidChange:(NSNotification *)notification;


@end
