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
#import <PlayerPROKit/PPErrors.h>
#import <PlayerPROKit/PPFXBusObject.h>
#import <PlayerPROKit/PPFXSetObject.h>
#if !TARGET_OS_IPHONE
#import <PlayerPROKit/PPPatternObject_PcmdHandling.h>
#import <PlayerPROKit/PPSoundSettingsViewController.h>
#if defined(ISUSINGPPPLUG)
#import <PlayerPROKit/PPPluginWindowController.h>
#import <PlayerPROKit/PPDigitalPluginWindowController.h>
#import <PlayerPROKit/PPFilterPluginWindowController.h>
#import <PlayerPROKit/PPInstrumentPluginWindowController.h>
#endif
#endif
