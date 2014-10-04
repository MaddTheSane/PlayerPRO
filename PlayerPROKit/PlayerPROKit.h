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

#import "PPLibrary.h"
#import "PPDriver.h"
#import "PPMusicObject.h"
#import "PPSampleObject.h"
#import "PPInstrumentObject.h"
#import "PPPatternObject.h"
#import "PPMadCommandObject.h"
#import "PPErrors.h"
#import "PPFXBusObject.h"
#import "PPFXSetObject.h"
#import "PPPatternObject_PcmdHandling.h"
#if !TARGET_OS_IPHONE
#import "PPSampleWaveformView.h"
#import "PPPasteboardHandling.h"
#import "PPPlugIns.h"
#endif
