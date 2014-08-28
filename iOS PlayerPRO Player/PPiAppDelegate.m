//
//  PPiAppDelegate.m
//  iOS PlayerPRO Player
//
//  Created by C.W. Betts on 9/19/13.
//
//

#import "PPiAppDelegate.h"
#import "UserDefaultKeys.h"
#import "iOS_PlayerPRO_Player-Swift.h"
//#import "PPMusicList.h"

@interface PPiAppDelegate ()
@property (readwrite, strong) PPLibrary *madLib;
@property (readwrite, strong) PPDriver *madDriver;
@end

@implementation PPiAppDelegate
@synthesize musicList;

+ (void)initialize
{
	static dispatch_once_t onceToken;
	dispatch_once(&onceToken, ^{
		[[NSUserDefaults standardUserDefaults] registerDefaults:@{PPRememberMusicList: @YES,
																  PPLoadMusicAtListLoad: @NO,
																  PPAfterPlayingMusic: @(PPStopPlaying),
																  PPGotoStartupAfterPlaying: @YES,
																  PPSaveModList: @YES,
																  PPLoadMusicAtMusicLoad: @NO,
																  PPLoopMusicWhenDone: @NO,
																  
																  PPSoundOutBits: @16,
																  PPSoundOutRate: @44100,
																  PPSoundDriver: @(CoreAudioDriver),
																  PPStereoDelayToggle: @YES,
																  PPReverbToggle: @NO,
																  PPSurroundToggle: @NO,
																  PPOversamplingToggle: @NO,
																  PPStereoDelayAmount: @30,
																  PPReverbAmount: @25,
																  PPReverbStrength: @30,
																  PPOversamplingAmount: @1,
																  
																  PPMAddExtension: @YES,
																  PPMMadCompression: @YES,
																  PPMNoLoadMixerFromFiles: @NO,
																  PPMOscilloscopeDrawLines: @YES}];
	});
}
	
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	// Override point for customization after application launch.
	if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
		UISplitViewController *splitViewController = (UISplitViewController *)self.window.rootViewController;
		UINavigationController *navigationController = [splitViewController.viewControllers lastObject];
		splitViewController.delegate = (id)navigationController.topViewController;
	}
	self.madLib = [[PPLibrary alloc] init];
	self.madDriver = [[PPDriver alloc] initWithLibrary:self.madLib settings:NULL];
	self.musicList = [[MusicList alloc] init];
	return YES;
}
							
- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
	[[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
	[[NSUserDefaults standardUserDefaults] synchronize];
}

@end
