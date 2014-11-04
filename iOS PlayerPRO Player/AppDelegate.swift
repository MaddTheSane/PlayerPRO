//
//  AppDelegate.swift
//  iOS Player
//
//  Created by C.W. Betts on 8/31/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import UIKit
import PlayerPROCore
import PlayerPROKit

enum PlaylistMode: Int {
	case StopPlaying = 0
	case LoopMusic
	case LoadNext
	case LoadRandom
};

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate, UISplitViewControllerDelegate {

	var window: UIWindow?
	let madLib = PPLibrary()
	var madDriver: PPDriver!
	var musicLists = [MusicList]()
	var musicFiles = [MusicListObject]()
	weak var musicList: MusicList!

	override init() {
		let tmpDict = [PPRememberMusicList: true,
		PPLoadMusicAtListLoad: false,
		PPAfterPlayingMusic: PlaylistMode.StopPlaying.rawValue,
		PPGotoStartupAfterPlaying: true,
		PPSaveModList: true,
		PPLoadMusicAtMusicLoad: false,
		PPLoopMusicWhenDone: false,
		
		PPSoundOutBits: 16,
		PPSoundOutRate: 44100,
		PPSoundDriver: Int(MADSoundOutput.CoreAudioDriver.rawValue),
		PPStereoDelayToggle: true,
		PPReverbToggle: false,
		PPSurroundToggle: false,
		PPOversamplingToggle: false,
		PPStereoDelayAmount: 30,
		PPReverbAmount: 25,
		PPReverbStrength: 30,
		PPOversamplingAmount: 1,
		
		PPMAddExtension: true,
		PPMMadCompression: true,
		PPMNoLoadMixerFromFiles: false,
		PPMOscilloscopeDrawLines: true]
		
		NSUserDefaults.standardUserDefaults().registerDefaults(tmpDict)
		
		var settings = MADDriverSettings()
		
		madDriver = PPDriver(library: madLib, settings: &settings, error: nil)

		super.init()
	}

	func application(application: UIApplication!, didFinishLaunchingWithOptions launchOptions: NSDictionary!) -> Bool {
		// Override point for customization after application launch.
		let splitViewController = self.window!.rootViewController as UISplitViewController
		let navigationController = splitViewController.viewControllers[splitViewController.viewControllers.count-1] as UINavigationController
		navigationController.topViewController.navigationItem.leftBarButtonItem = splitViewController.displayModeButtonItem()
		splitViewController.delegate = self
		
		return true
	}

	func applicationWillResignActive(application: UIApplication!) {
		// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
		// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
	}

	func applicationDidEnterBackground(application: UIApplication!) {
		// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
		// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
	}

	func applicationWillEnterForeground(application: UIApplication!) {
		// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
	}

	func applicationDidBecomeActive(application: UIApplication!) {
		// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
	}

	func applicationWillTerminate(application: UIApplication!) {
		// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
	}

	// MARK: - Split view

	func splitViewController(splitViewController: UISplitViewController!, collapseSecondaryViewController secondaryViewController:UIViewController!, ontoPrimaryViewController primaryViewController:UIViewController!) -> Bool {
	    if let secondaryAsNavController = secondaryViewController as? UINavigationController {
	        if let topAsDetailController = secondaryAsNavController.topViewController as? DetailViewController {
	            if topAsDetailController.detailItem == nil {
	                // Return true to indicate that we have handled the collapse by doing nothing; the secondary controller will be discarded.
	                return true
	            }
	        }
	    }
	    return false
	}

}

