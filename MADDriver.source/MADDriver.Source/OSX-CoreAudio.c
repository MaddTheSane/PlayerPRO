/*
 *  Mac-CoreAudio.c
 *  PlayerPRO tryout
 *
 *  Created by C.W. Betts on 6/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "RDriver.h"
#include "RDriverInt.h"
#include "PPPrivate.h"

//TODO: we should probably do something to prevent thread contention
static OSStatus     CAAudioCallback (void                            *inRefCon,
									 AudioUnitRenderActionFlags      *ioActionFlags,
									 const AudioTimeStamp            *inTimeStamp,
									 UInt32                          inBusNumber,
									 UInt32                          inNumberFrames,
									 AudioBufferList                 *ioData)
{
	MADDriverRec *theRec = (MADDriverRec*)inRefCon;
	//int j = 0;
	if(theRec->Reading == false)
	{
		switch( theRec->DriverSettings.outPutBits)
		{
			case 8:
				memset(theRec->CABuffer, 0x80, theRec->BufSize);
				break;
							
			case 16:
				memset(theRec->CABuffer, 0, theRec->BufSize);
				break;
		}
	}

	UInt32 remaining, len;
	AudioBuffer *abuf;
	void *ptr;
	UInt32 i = 0;
	for (i = 0; i < ioData->mNumberBuffers; i++) {
        abuf = &ioData->mBuffers[i];
        remaining = abuf->mDataByteSize;
        ptr = abuf->mData;
        while (remaining > 0) {
            if (theRec->CABufOff >= theRec->BufSize) {
                if( !DirectSave( theRec->CABuffer, NULL, theRec))
				{
					switch( theRec->DriverSettings.outPutBits)
					{
						case 8:
							memset(theRec->CABuffer, 0x80, theRec->BufSize);
							break;
							
						case 16:
							memset(theRec->CABuffer, 0, theRec->BufSize);
							break;
					}
				}
				theRec->CABufOff = 0;
            }
			
            len = theRec->BufSize - theRec->CABufOff;
            if (len > remaining)
                len = remaining;
            memcpy(ptr, (char *)theRec->CABuffer + theRec->CABufOff, len);
            ptr = (char *)ptr + len;
            remaining -= len;
            theRec->CABufOff += len;
        }
    }
	
	/*if( BuffSize - pos > tickadd)	theRec->OscilloWavePtr = theRec->CABuffer + (int)pos;
	else */ theRec->OscilloWavePtr = theRec->CABuffer;
	return noErr;
}

OSErr initCoreAudio( MADDriverRec *inMADDriver)
{
	OSStatus result = noErr;
	struct AURenderCallbackStruct callback, blankCallback;
	callback.inputProc = CAAudioCallback;
	callback.inputProcRefCon = inMADDriver;
	
	blankCallback.inputProc = NULL;
	blankCallback.inputProcRefCon = NULL;

	AudioComponentDescription theDes;
	theDes.componentType = kAudioUnitType_Output;
#if TARGET_OS_IPHONE
	theDes.componentSubType = kAudioUnitSubType_GenericOutput;
#else
	theDes.componentSubType = kAudioUnitSubType_DefaultOutput;
#endif
	theDes.componentManufacturer = kAudioUnitManufacturer_Apple;
	theDes.componentFlags = 0;
	theDes.componentFlagsMask = 0;
	AudioStreamBasicDescription audDes = {0};
	audDes.mFormatID = kAudioFormatLinearPCM;
	audDes.mFormatFlags = kLinearPCMFormatFlagIsPacked;
	audDes.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger;

	int outChn = 0;
	switch (inMADDriver->DriverSettings.outPutMode) {
		case MonoOutPut:
			outChn = 1;
			break;
			
		case StereoOutPut:
		case DeluxeStereoOutPut:
		default:
			outChn = 2;
			break;
			
		case PolyPhonic:
			outChn = 4;
			break;
	}
	audDes.mChannelsPerFrame = outChn;
	audDes.mSampleRate = inMADDriver->DriverSettings.outPutRate;
	audDes.mBitsPerChannel = inMADDriver->DriverSettings.outPutBits;
	audDes.mFramesPerPacket = 1;
    audDes.mBytesPerFrame = audDes.mBitsPerChannel * audDes.mChannelsPerFrame / 8;
    audDes.mBytesPerPacket = audDes.mBytesPerFrame * audDes.mFramesPerPacket;

	
	AudioComponent theComp = AudioComponentFindNext(NULL, &theDes);
	if (theComp == NULL) {
		return MADSoundManagerErr;
	}
	result = AudioComponentInstanceNew(theComp, &inMADDriver->CAAudioUnit);
	if (result != noErr) {
		return MADSoundManagerErr;
	}
	
	result = AudioUnitSetProperty (inMADDriver->CAAudioUnit,
								   kAudioUnitProperty_StreamFormat,
								   kAudioUnitScope_Input,
								   0,
								   &audDes,
								   sizeof (audDes));
	if (result != noErr) {
		AudioComponentInstanceDispose(inMADDriver->CAAudioUnit);
		return MADSoundManagerErr;
	}
	
	result = AudioUnitSetProperty(inMADDriver->CAAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callback, sizeof(callback));
	if (result != noErr) {
		AudioComponentInstanceDispose(inMADDriver->CAAudioUnit);
		return MADSoundManagerErr;
	}
	
	result = AudioUnitInitialize(inMADDriver->CAAudioUnit);
	if (result != noErr) {
		AudioComponentInstanceDispose(inMADDriver->CAAudioUnit);
		AudioUnitSetProperty(inMADDriver->CAAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &blankCallback, sizeof(blankCallback));
		return MADSoundManagerErr;
	}
	
	inMADDriver->CABufOff = inMADDriver->BufSize;
	inMADDriver->CABuffer = calloc(inMADDriver->BufSize, 1);
	
	result = AudioOutputUnitStart(inMADDriver->CAAudioUnit);
	if (result != noErr) {
		free(inMADDriver->CABuffer);
		AudioComponentInstanceDispose(inMADDriver->CAAudioUnit);
		AudioUnitSetProperty(inMADDriver->CAAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &blankCallback, sizeof(blankCallback));
		return MADSoundManagerErr;
	}
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
	inMADDriver->OscilloWavePtr = NULL;
	if (inMADDriver->CABuffer) {
		free(inMADDriver->CABuffer);
	}
	return noErr;
}
