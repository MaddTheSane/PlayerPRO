//
//  PPiAppDelegate.h
//  iOS PlayerPRO Player
//
//  Created by C.W. Betts on 9/19/13.
//
//

#import <UIKit/UIKit.h>
@import PlayerPROKit;

@class PPMusicList;

enum {
	PPStopPlaying = 0,
	PPLoopMusic,
	PPLoadNext,
	PPLoadRandom
};

@interface PPiAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (readonly, strong) PPLibrary *madLib;
@property (readonly, strong) PPDriver *madDriver;
@property (strong, nonatomic) PPMusicList *musicList;

@end
