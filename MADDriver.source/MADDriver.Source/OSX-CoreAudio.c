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

static Ptr CABuffer = NULL;
static Boolean OnOff = false;
static size_t BuffSize = 0;

static Boolean WriteDataToBuffer(
								 AudioBufferList *ioData,
								 size_t bufferOffset,
								 Ptr buffer,
								 size_t bufferSize)
{
    UInt32 remaining, len;
    AudioBuffer *abuf;
    void *ptr;
    UInt32 i;
	for (i = 0; i < ioData->mNumberBuffers; i++) {
        abuf = &ioData->mBuffers[i];
        remaining = abuf->mDataByteSize;
        ptr = abuf->mData;
        while (remaining > 0) {
            if (bufferOffset >= bufferSize) {
                bufferOffset = 0;
            }
			
            len = bufferSize - bufferOffset;
            if (len > remaining)
                len = remaining;
            memcpy(ptr, (char *)buffer + bufferOffset, len);
            ptr = (char *)ptr + len;
            remaining -= len;
            bufferOffset += len;
        }
    }
	return true;
}

//TODO: we should probably do something to prevent thread contention
static OSStatus     CAAudioCallback (void                            *inRefCon,
									 AudioUnitRenderActionFlags      *ioActionFlags,
									 const AudioTimeStamp            *inTimeStamp,
									 UInt32                          inBusNumber,
									 UInt32                          inNumberFrames,
									 AudioBufferList                 *ioData)
{
	MADDriverRec *theRec = (MADDriverRec*)inRefCon;
		
	//WinMADDriver->lpSwSamp->lpVtbl->GetCurrentPosition( WinMADDriver->lpSwSamp, &pos, &posp);
	static Boolean tick = false;
	const int tickadd = 1884;
	//int pos = 4095;
	
	//pos = inTimeStamp->mSampleTime;
	
	int i = 0;
	
	//if(pos > BuffSize/2 && OnOff == true)
	{
		OnOff = false;
		
		if( !DirectSave( CABuffer, NULL, theRec))
		{
			switch( theRec->DriverSettings.outPutBits)
			{
				case 8:
					for( i = 0; i < BuffSize/2; i++) CABuffer[ i] = 0x80;
					break;
					
				case 16:
					for( i = 0; i < BuffSize/2; i++) CABuffer[ i] = 0;
					break;
			}
		}
		if (tick == false)
		{
			if( !WriteDataToBuffer( ioData, 0, CABuffer, BuffSize/2))
			{
				//DEBUG 	debugger("ERR");
			}
			tick = true;
		} else {
			if( !WriteDataToBuffer( ioData, tickadd, CABuffer + tickadd, BuffSize/2 - tickadd))
			{
				//DEBUG 	debugger("ERR");
			}
			tick = false;
		}
	}
		
	/*else if( OnOff == false && (pos < BuffSize/2))
	{
		OnOff = true;
		
		if( !DirectSave( CABuffer + BuffSize/2, NULL, theRec))
		{
			switch( theRec->DriverSettings.outPutBits)
			{
				case 8:
					for( i = BuffSize/2; i < BuffSize; i++) CABuffer[ i] = 0x80;
					break;
					
				case 16:
					for( i = BuffSize/2; i < BuffSize; i++) CABuffer[ i] = 0;
					break;
			}
		}
		
		if( !WriteDataToBuffer( ioData, BuffSize/2, (unsigned char*) (CABuffer + BuffSize/2), BuffSize/2))
		{
			//DEBUG 	debugger("ERR");
		}
	}
	
	if( BuffSize - pos > tickadd)	theRec->OscilloWavePtr = CABuffer + (int)pos; 
	else */ theRec->OscilloWavePtr = CABuffer;
	return noErr;
}

OSErr initCoreAudio( MADDriverRec *inMADDriver)
{
	OSStatus result = noErr;
	struct AURenderCallbackStruct callback;
	callback.inputProc = CAAudioCallback;
	callback.inputProcRefCon = inMADDriver;
	AudioComponentDescription theDes;
	theDes.componentType = kAudioUnitType_Output;
    theDes.componentSubType = kAudioUnitSubType_DefaultOutput;
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
		return MADUnknownErr;
	}
	result = AudioComponentInstanceNew(theComp, &inMADDriver->CAAudioUnit);
	
	
	result = AudioUnitSetProperty (inMADDriver->CAAudioUnit,
								   kAudioUnitProperty_StreamFormat,
								   kAudioUnitScope_Input,
								   0,
								   &audDes,
								   sizeof (audDes));
	
	result = AudioUnitSetProperty(inMADDriver->CAAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callback, sizeof(callback));

	
	result = AudioUnitInitialize(inMADDriver->CAAudioUnit);
	
	result = AudioOutputUnitStart(inMADDriver->CAAudioUnit);
	BuffSize = inMADDriver->BufSize * 2;
	CABuffer = calloc(BuffSize, 1);
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
	if (CABuffer) {
		free(CABuffer);
	}
	return noErr;
}
