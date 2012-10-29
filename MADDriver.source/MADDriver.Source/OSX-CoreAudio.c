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
	OSStatus result = noErr;
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
	audDes.mBitsPerChannel = inMADDriver->DriverSettings.outPutBits;
	audDes.mFramesPerPacket = 1;
    audDes.mBytesPerFrame = audDes.mBitsPerChannel * audDes.mChannelsPerFrame / 8;
    audDes.mBytesPerPacket = audDes.mBytesPerFrame * audDes.mFramesPerPacket;

	
	AudioComponent theComp = AudioComponentFindNext(NULL, &theDes);
	if (theComp == NULL) {
		return MADUnknownErr;
	}
	result = AudioComponentInstanceNew(theComp, &inMADDriver->CAAudioUnit);
	
	
	result = AudioUnitSetProperty (inMADDriver->CAAudioUnit,
								   kAudioUnitProperty_StreamFormat,
								   kAudioUnitScope_Input,
								   0,
								   &audDes,
								   sizeof (audDes));

	
	result = AudioUnitInitialize(inMADDriver->CAAudioUnit);
	
#if 0
	result = AudioOutputUnitStart(inMADDriver->CAAudioUnit);
#endif
	
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
