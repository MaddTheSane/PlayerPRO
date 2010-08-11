//
//  MusicListPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MusicListPreferenceController : NSViewController {
    IBOutlet NSButton *askToSave;
    IBOutlet NSButton *autoPlay;
    IBOutlet NSButton *goToStartupPos;
    IBOutlet NSButton *loadFirstMusic;
    IBOutlet NSButtonCell *musicPlayingOutlet;
    IBOutlet NSButton *rememberMusicList;
    IBOutlet NSButton *showIcons;
}
- (IBAction)changePlayedMusic:(id)sender;
- (IBAction)toggleButton:(id)sender;
@end
