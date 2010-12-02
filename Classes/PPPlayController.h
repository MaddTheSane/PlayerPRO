//
//  PPPlayController.h
//
//  Created by C.W. Betts on 11/23/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "PlayerPROCore.h"

@interface PPPlayController : NSObject {
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
	
}
- (IBAction)fastForwardButtonPressed:(id)sender;
- (IBAction)loopButtonPressed:(id)sender;
- (IBAction)nextButtonPressed:(id)sender;
- (IBAction)playButtonPressed:(id)sender;
- (IBAction)prevButtonPressed:(id)sender;
- (IBAction)recordButtonPressed:(id)sender;
- (IBAction)rewindButtonPressed:(id)sender;
- (IBAction)sliderChanged:(id)sender;
- (IBAction)stopButtonPressed:(id)sender;

-(void)soundPreferencesDidChange:(NSNotification *)notification;

@end
