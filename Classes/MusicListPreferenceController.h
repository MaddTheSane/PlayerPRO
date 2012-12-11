//
//  MusicListPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MusicListPreferenceController : NSViewController {
    IBOutlet NSButtonCell *askToSave;
    IBOutlet NSButtonCell *autoPlay;
    IBOutlet NSButtonCell *goToStartupPos;
    IBOutlet NSButtonCell *loadFirstMusic;
    IBOutlet NSMatrix *musicPlayingOutlet;
    IBOutlet NSButtonCell *rememberMusicList;
	IBOutlet NSButtonCell *loopMusic;
}
- (IBAction)changePlayedMusic:(id)sender;
- (IBAction)toggleButton:(id)sender;
@end
