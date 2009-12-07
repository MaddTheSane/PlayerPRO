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

OSErr PPIdentifyFSRef( MADLibrary *inMADDriver, char *type, FSRefPtr AlienRef);

#pragma mark Core Audio Functions
OSErr initCoreAudio( MADDriverRec *inMADDriver, long init);
OSErr closeCoreAudio( MADDriverRec *inMADDriver);
void StopChannelCA(MADDriverRec *inMADDriver);
void PlayChannelCA(MADDriverRec *inMADDriver);

#pragma mark CoreMIDI Functions
OSErr initCoreMIDI( MADDriverRec *inMADDriver, long init);
OSErr closeCoreMIDI( MADDriverRec *inMADDriver);
void StopChannelCoreMIDI(MADDriverRec *inMADDriver);
void PlayChannelCoreMIDI(MADDriverRec *inMADDriver);



#endif
