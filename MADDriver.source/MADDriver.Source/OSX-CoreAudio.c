/*
 *  Mac-CoreAudio.c
 *  PlayerPRO tryout
 *
 *  Created by C.W. Betts on 6/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <CoreAudio/CoreAudio.h>
#include <Carbon/Carbon.h>
#include "RDriver.h"
#include "PPPrivate.h"

//TODO: Make CoreAudio back end
#warning Nothing implemented here yet

OSErr initCoreAudio( MADDriverRec *inMADDriver, long init)
{
	return -8;
}

OSErr closeCoreAudio( MADDriverRec *inMADDriver)
{
	return -8;
}

void StopChannelCA(MADDriverRec *inMADDriver)
{
	
}

void PlayChannelCA(MADDriverRec *inMADDriver)
{
	
}

//TODO: Make CoreMIDI back end
#include <CoreMIDI/CoreMIDI.h>

OSErr initCoreMIDI( MADDriverRec *inMADDriver, long init)
{
	return -8;
}

OSErr closeCoreMIDI( MADDriverRec *inMADDriver)
{
	return -8;
}

void StopChannelCoreMIDI(MADDriverRec *inMADDriver)
{
	
}

void PlayChannelCoreMIDI(MADDriverRec *inMADDriver)
{
	
}
