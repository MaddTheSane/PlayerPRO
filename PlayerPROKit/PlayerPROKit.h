//
//  PlayerPROKit.h
//  PlayerPROKit
//
//  Created by C.W. Betts on 10/28/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

//! Project version number for PlayerPROKit.
FOUNDATION_EXPORT double PlayerPROKit_VersionNumber;

//! Project version string for PlayerPROKit.
FOUNDATION_EXPORT const unsigned char PlayerPROKit_VersionString[];

#import <PlayerPROKit/PPLibrary.h>
#import <PlayerPROKit/PPDriver.h>
#import <PlayerPROKit/PPMusicObject.h>
#import <PlayerPROKit/PPSampleObject.h>
#import <PlayerPROKit/PPInstrumentObject.h>
#import <PlayerPROKit/PPPatternObject.h>
#import <PlayerPROKit/PPMadCommandObject.h>
#import <PlayerPROKit/PPErrors.h>
#import <PlayerPROKit/PPFXBusObject.h>
#import <PlayerPROKit/PPFXSetObject.h>
#import <PlayerPROKit/PPPatternObject_PcmdHandling.h>
#if !TARGET_OS_IPHONE
#import <PlayerPROKit/PPSoundSettingsViewController.h>
#import <PlayerPROKit/PPPluginWindowController.h>
#import <PlayerPROKit/PPDigitalPluginWindowController.h>
#import <PlayerPROKit/PPFilterPluginWindowController.h>
#import <PlayerPROKit/PPInstrumentPluginWindowController.h>
#endif
