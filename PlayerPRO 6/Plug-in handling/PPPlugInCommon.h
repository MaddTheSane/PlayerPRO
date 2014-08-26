//
//  PPPlugInCommon.h
//  PPMacho
//
//  Created by C.W. Betts on 12/26/12.
//
//

#ifndef PPMacho_PPPlugInCommon_h
#define PPMacho_PPPlugInCommon_h

#include <PlayerPROCore/PlayerPROCore.h>
#import <Foundation/NSArray.h>

void **GetCOMPlugInterface(CFBundleRef tempBundleRef, CFUUIDRef TypeUUID, CFUUIDRef InterfaceUUID);
NSArray *DefaultPlugInLocations();
OSErr inMADPlaySoundData(MADDriverRec *theRec, Ptr soundPtr, long size, int channel, int note, int amplitude, long loopBeg, long loopSize, unsigned int rate, Boolean stereo);

#endif
