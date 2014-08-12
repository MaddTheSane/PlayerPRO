//
//  MusicListPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MusicListPreferenceController : NSViewController
@property (weak) IBOutlet NSButtonCell *askToSave;
@property (weak) IBOutlet NSButtonCell *autoPlay;
@property (weak) IBOutlet NSButtonCell *goToStartupPos;
@property (weak) IBOutlet NSButtonCell *loadFirstMusic;
@property (weak) IBOutlet NSMatrix *musicPlayingOutlet;
@property (weak) IBOutlet NSButtonCell *rememberMusicList;
@property (weak) IBOutlet NSButtonCell *loopMusic;

- (IBAction)changePlayedMusic:(id)sender;
- (IBAction)toggleButton:(id)sender;

+ (instancetype)newPreferenceView;
@end
