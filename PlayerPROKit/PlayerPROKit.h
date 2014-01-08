//
//  PlayerPROKit.h
//  PlayerPROKit
//
//  Created by C.W. Betts on 10/28/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

#import <PlayerPROKit/PPLibrary.h>
#import <PlayerPROKit/PPDriver.h>
#import <PlayerPROKit/PPMusicObject.h>
#import <PlayerPROKit/PPSampleObject.h>
#import <PlayerPROKit/PPInstrumentObject.h>
#import <PlayerPROKit/PPPatternObject.h>
#import <PlayerPROKit/PPMadCommandObject.h>
#if !TARGET_OS_IPHONE
#import <PlayerPROKit/PPSoundSettingsViewController.h>
#if defined(ISUSINGPPPLUG)
#import <PlayerPROKit/PPPluginWindowController.h>
#import <PlayerPROKit/PPDigitalPluginWindowController.h>
#import <PlayerPROKit/PPFilterPluginWindowController.h>
#import <PlayerPROKit/PPInstrumentPluginWindowController.h>
#endif
#endif
