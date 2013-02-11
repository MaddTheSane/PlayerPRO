//
//  MusicListPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MusicListPreferenceController : NSViewController {
    IBOutlet __weak NSButtonCell *askToSave;
    IBOutlet __weak NSButtonCell *autoPlay;
    IBOutlet __weak NSButtonCell *goToStartupPos;
    IBOutlet __weak NSButtonCell *loadFirstMusic;
    IBOutlet __weak NSMatrix *musicPlayingOutlet;
    IBOutlet __weak NSButtonCell *rememberMusicList;
	IBOutlet __weak NSButtonCell *loopMusic;
}
- (IBAction)changePlayedMusic:(id)sender;
- (IBAction)toggleButton:(id)sender;
@end
