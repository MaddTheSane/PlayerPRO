/*
 *  PPPrivate.h
 *  PlayerPROCore
 *
 *  Created by C.W. Betts on 6/30/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __PLAYERPROCORE_PPPRIVATE__
#define __PLAYERPROCORE_PPPRIVATE__

#include "RDriver.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MAC_H
#pragma mark Core Audio Functions
OSErr initCoreAudio( MADDriverRec *inMADDriver, long init);
OSErr closeCoreAudio( MADDriverRec *inMADDriver);
void StopChannelCA(MADDriverRec *inMADDriver);
void PlayChannelCA(MADDriverRec *inMADDriver);

CFMutableArrayRef GetDefaultPluginFolderLocations();

void GetPStrFromCFString(const CFStringRef source, StringPtr pStrOut);
void **GetCOMPlugInterface(CFPlugInRef plugToTest, CFUUIDRef TypeUUID, CFUUIDRef InterfaceUUID);


const CFStringRef kMadPlugMenuNameKey;
const CFStringRef kMadPlugAuthorNameKey;
const CFStringRef kMadPlugUTITypesKey;
const CFStringRef kMadPlugModeKey;
const CFStringRef kMadPlugTypeKey;
#endif

#ifdef _ESOUND
OSErr initESD( MADDriverRec *inMADDriver);
OSErr closeESD( MADDriverRec *inMADDriver);
void StopChannelESD(MADDriverRec *inMADDriver);
void PlayChannelESD(MADDriverRec *inMADDriver);
#endif

#ifdef _OSSSOUND
OSErr initOSS( MADDriverRec *inMADDriver);
OSErr closeOSS( MADDriverRec *inMADDriver);
void StopChannelOSS(MADDriverRec *inMADDriver);
void PlayChannelOSS(MADDriverRec *inMADDriver);
#endif

#ifdef LINUX
OSErr initALSA( MADDriverRec *inMADDriver);
OSErr closeALSA( MADDriverRec *inMADDriver);
void StopChannelALSA(MADDriverRec *inMADDriver);
void PlayChannelALSA(MADDriverRec *inMADDriver);
#endif

#ifdef __cplusplus
}
#endif

#endif
