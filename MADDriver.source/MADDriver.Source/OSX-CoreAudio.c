/*
 *  Mac-CoreAudio.c
 *  PlayerPRO tryout
 *
 *  Created by C.W. Betts on 6/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include "RDriver.h"
#include "RDriverInt.h"
#include "PPPrivate.h"

//TODO: we should probably do something to prevent thread contention
static OSStatus CAAudioCallback(void                            *inRefCon,
								AudioUnitRenderActionFlags      *ioActionFlags,
								const AudioTimeStamp            *inTimeStamp,
								UInt32                          inBusNumber,
								UInt32                          inNumberFrames,
								AudioBufferList                 *ioData)
{
	MADDriverRec *theRec = (MADDriverRec*)inRefCon;
	if(theRec->Reading == false) {
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
                if(!DirectSave(theRec->CABuffer, NULL, theRec)) {
					switch(theRec->DriverSettings.outPutBits)
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
            memcpy(ptr, (void*)((size_t)theRec->CABuffer + theRec->CABufOff), len);
            ptr = (Ptr)((size_t)ptr + len);
            remaining -= len;
            theRec->CABufOff += len;
        }
    }
	
	/*if (BuffSize - pos > tickadd)	theRec->OscilloWavePtr = theRec->CABuffer + (int)pos;
	 else */ theRec->OscilloWavePtr = theRec->CABuffer;
	return noErr;
}

OSErr initCoreAudio( MADDriverRec *inMADDriver, long init)
{
	OSStatus result = noErr;
	struct AURenderCallbackStruct callback, blankCallback = {0};
	callback.inputProc = CAAudioCallback;
	callback.inputProcRefCon = inMADDriver;
	
	ComponentDescription theDes = {0};
	theDes.componentType = kAudioUnitType_Output;
	theDes.componentSubType = kAudioUnitSubType_DefaultOutput;
	theDes.componentManufacturer = kAudioUnitManufacturer_Apple;
	//theDes.componentFlags = 0;
	//theDes.componentFlagsMask = 0;
	AudioStreamBasicDescription audDes = {0};
	audDes.mFormatID = kAudioFormatLinearPCM;
	audDes.mFormatFlags = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsSignedInteger;
#ifdef __BIG_ENDIAN__
	audDes.mFormatFlags |= kLinearPCMFormatFlagIsBigEndian;
#endif
	
	switch (inMADDriver->DriverSettings.outPutMode) {
		case MonoOutPut:
			audDes.mChannelsPerFrame = 1;
			break;
			
		case StereoOutPut:
		case DeluxeStereoOutPut:
		default:
			audDes.mChannelsPerFrame = 2;
			break;
			
		case PolyPhonic:
			audDes.mChannelsPerFrame = 4;
			break;
	}
	
	audDes.mSampleRate = FixedToFloat(inMADDriver->DriverSettings.outPutRate);
	audDes.mBitsPerChannel = inMADDriver->DriverSettings.outPutBits;
	audDes.mFramesPerPacket = 1;
	audDes.mBytesPerFrame = audDes.mBitsPerChannel * audDes.mChannelsPerFrame / 8;
	audDes.mBytesPerPacket = audDes.mBytesPerFrame * audDes.mFramesPerPacket;
	
	Component theComp = FindNextComponent(NULL, &theDes);
	if (theComp == NULL) {
		return MADSoundManagerErr;
	}
	result = OpenAComponent(theComp, &inMADDriver->CAAudioUnit);
	if (result != noErr) {
		return MADSoundManagerErr;
	}
	
	result = AudioUnitSetProperty(inMADDriver->CAAudioUnit,
								  kAudioUnitProperty_StreamFormat,
								  kAudioUnitScope_Input,
								  0,
								  &audDes,
								  sizeof(audDes));
	if (result != noErr) {
		CloseComponent(inMADDriver->CAAudioUnit);
		return MADSoundManagerErr;
	}
	
	result = AudioUnitSetProperty(inMADDriver->CAAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callback, sizeof(callback));
	if (result != noErr) {
		CloseComponent(inMADDriver->CAAudioUnit);
		return MADSoundManagerErr;
	}
	
	result = AudioUnitInitialize(inMADDriver->CAAudioUnit);
	if (result != noErr) {
		AudioUnitSetProperty(inMADDriver->CAAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &blankCallback, sizeof(blankCallback));
		CloseComponent(inMADDriver->CAAudioUnit);
		return MADSoundManagerErr;
	}
	
	inMADDriver->CABufOff = inMADDriver->BufSize;
	inMADDriver->CABuffer = NewPtrClear(inMADDriver->BufSize);
	
	result = AudioOutputUnitStart(inMADDriver->CAAudioUnit);
	if (result != noErr) {
		DisposePtr(inMADDriver->CABuffer);
		AudioUnitSetProperty(inMADDriver->CAAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &blankCallback, sizeof(blankCallback));
		CloseComponent(inMADDriver->CAAudioUnit);
		return MADSoundManagerErr;
	}
	return noErr;
}

OSErr closeCoreAudio( MADDriverRec *inMADDriver)
{
	struct AURenderCallbackStruct callback = {0};
	
	OSStatus result = AudioOutputUnitStop(inMADDriver->CAAudioUnit);
	if (result != noErr) {
		
	}
	result = AudioUnitSetProperty(inMADDriver->CAAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callback, sizeof(callback));
	if (result != noErr)
	{
		
	}
	result = CloseComponent(inMADDriver->CAAudioUnit);
	if (result != noErr) {
		
	}
	inMADDriver->OscilloWavePtr = NULL;
	if (inMADDriver->CABuffer) {
		DisposePtr(inMADDriver->CABuffer);
	}
	return noErr;
}
