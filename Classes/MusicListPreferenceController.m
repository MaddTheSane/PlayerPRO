//
//  MusicListPreferenceController.m
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MusicListPreferenceController.h"
#import "UserDefaultKeys.h"
#import "PPPreferences.h"

@implementation MusicListPreferenceController

-(NSInteger)playedMusic {
	id curSelected = [musicPlayingOutlet selectedCell];
	if ([musicPlayingOutlet cellAtRow:0 column:0] == curSelected) {
		return PPStopPlaying;
	} else if ([musicPlayingOutlet cellAtRow:0 column:1] == curSelected) {
		return PPLoadNext;
	} else if ([musicPlayingOutlet cellAtRow:1 column:0] == curSelected) {
		return PPLoopMusic;
	} else if ([musicPlayingOutlet cellAtRow:1 column:1] == curSelected) {
		return PPLoadRandom;
	} else {
		return -1;
	}	
}

- (IBAction)changePlayedMusic:(id)sender {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setInteger:[self playedMusic] forKey:PPAfterPlayingMusic];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPListPreferencesDidChange object:self];
}

- (IBAction)toggleButton:(id)sender {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setBool:[rememberMusicList state] forKey:PPRememberMusicList];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:PPListPreferencesDidChange object:self];
}

-(id)init {
	if (self = [super initWithNibName:@"MusicListPrefs" bundle:nil]) {
		[self setTitle:@"Music List"];
	}
	return self;
}

-(void)setPlayedMusic:(NSUInteger)toSet {
	switch (toSet) {
		case PPStopPlaying:
			[musicPlayingOutlet selectCellAtRow:0 column:0];
			break;
		case PPLoadNext:
			[musicPlayingOutlet selectCellAtRow:0 column:1];
			break;
		case PPLoopMusic:
			[musicPlayingOutlet selectCellAtRow:1 column:0];
			break;
		case PPLoadRandom:
			[musicPlayingOutlet selectCellAtRow:1 column:1];
			break;

		default:
			[musicPlayingOutlet selectCellAtRow:0 column:0];
			break;
	}
	
}

-(void)awakeFromNib {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	[rememberMusicList setState:[defaults boolForKey:PPRememberMusicList]];
	[loadFirstMusic setState:[defaults boolForKey:PPLoadMusicAtListLoad]];
	[goToStartupPos setState:[defaults boolForKey:PPGotoStartupAfterPlaying]];
	[askToSave setState:[defaults boolForKey:PPSaveModList]];
	[autoPlay setState:[defaults boolForKey:PPLoadMusicAtMusicLoad]];
	[self setPlayedMusic:[defaults integerForKey:PPAfterPlayingMusic]];
}

@end
