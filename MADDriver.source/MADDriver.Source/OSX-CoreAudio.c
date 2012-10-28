/*
 *  Mac-CoreAudio.c
 *  PlayerPRO tryout
 *
 *  Created by C.W. Betts on 6/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <CoreAudio/CoreAudio.h>
#include "RDriver.h"
#include "RDriverInt.h"
#include "PPPrivate.h"

//TODO: Make CoreAudio back end
#warning Nothing implemented here yet

OSErr initCoreAudio( MADDriverRec *inMADDriver)
{
	AudioComponentDescription theDes;
	theDes.componentType = kAudioUnitType_Output;
    theDes.componentSubType = kAudioUnitSubType_DefaultOutput;
    theDes.componentManufacturer = kAudioUnitManufacturer_Apple;
    theDes.componentFlags = 0;
    theDes.componentFlagsMask = 0;
	AudioStreamBasicDescription audDes = {0};
	audDes.mFormatID = kAudioFormatLinearPCM;
	audDes.mFormatFlags = kLinearPCMFormatFlagIsPacked;
	audDes.mChannelsPerFrame = inMADDriver->DriverSettings.numChn;
	audDes.mSampleRate = inMADDriver->DriverSettings.outPutRate;
	
	AudioComponent theComp = AudioComponentFindNext(NULL, &theDes);
	AudioComponentInstanceNew(theComp, &inMADDriver->CAAudioUnit);
	
	
	AudioUnitSetProperty (inMADDriver->CAAudioUnit,
						  kAudioUnitProperty_StreamFormat,
						  kAudioUnitScope_Input,
						  0,
						  &audDes,
						  sizeof (audDes));

	
	AudioUnitInitialize(inMADDriver->CAAudioUnit);
	
	return noErr;
}

OSErr closeCoreAudio( MADDriverRec *inMADDriver)
{
	struct AURenderCallbackStruct callback;
	callback.inputProc = NULL;
	callback.inputProcRefCon = NULL;
	
	OSStatus result = AudioOutputUnitStop(inMADDriver->CAAudioUnit);
	if (result != noErr) {
		
	}
	result = AudioUnitSetProperty(inMADDriver->CAAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callback, sizeof(callback));
	if (result != noErr)
	{
		
	}
	result = AudioComponentInstanceDispose(inMADDriver->CAAudioUnit);
	if (result != noErr) {
		
	}
	return noErr;
}

void StopChannelCA(MADDriverRec *inMADDriver)
{
	
}

void PlayChannelCA(MADDriverRec *inMADDriver)
{
	
}
