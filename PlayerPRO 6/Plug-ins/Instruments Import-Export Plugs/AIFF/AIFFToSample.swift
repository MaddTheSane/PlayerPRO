//
//  AIFFToSample.swift
//  PPMacho
//
//  Created by C.W. Betts on 5/11/15.
//
//

import Foundation
import AudioToolbox
import PlayerPROCore
import PlayerPROKit
import SwiftAdditions
import SwiftAudioAdditions

private let kSrcBufSize: UInt32 = 32768;


internal func AIFFAtURL(url: NSURL, toSample sample: PPSampleObject) -> MADErr {
	var fileRef: ExtAudioFileRef = nil
	var iErr = ExtAudioFileOpenURL(url, &fileRef)
	if iErr != noErr {
		return .ReadingErr
	}
	if let mutableData = NSMutableData(capacity: Int(kSrcBufSize) * 8) {
		var realFormat = AudioStreamBasicDescription()
		
		var asbdSize = UInt32(sizeof(AudioStreamBasicDescription))
		iErr = ExtAudioFileGetProperty(fileRef, propertyID: .FileDataFormat, propertyDataSize: &asbdSize, propertyData: &realFormat)
		if iErr != noErr {
			ExtAudioFileDispose(fileRef)
			
			return .UnknownErr
		}
		
		//Constrain the audio conversion to values supported by PlayerPRO
		realFormat.mSampleRate = ceil(realFormat.mSampleRate)
		realFormat.mSampleRate = clamp(realFormat.mSampleRate, minimum: 5000, maximum: 44100)
		realFormat.formatFlags = .NativeEndian | .Packed | .SignedInteger
		switch realFormat.mBitsPerChannel {
		case 8, 16:
			break
		case 1...7:
			realFormat.mBitsPerChannel = 8
			//case 9...15:
			//	realFormat.mBitsPerChannel = 16
			//case 20, 24:
		default:
			realFormat.mBitsPerChannel = 16
		}
		
		switch realFormat.mChannelsPerFrame {
		case 1, 2:
			break
		default:
			realFormat.mChannelsPerFrame = 2
		}
		
		iErr = ExtAudioFileSetProperty(fileRef, propertyID: .ClientDataFormat, dataSize: sizeof(AudioStreamBasicDescription), data: &realFormat)
		if iErr != noErr {
			ExtAudioFileDispose(fileRef)
			return .UnknownErr
		}
		
		while true {
			if let tmpMutDat = NSMutableData(length: Int(kSrcBufSize)) {
				var fillBufList = AudioBufferList.allocate(maximumBuffers: 1)
				var err: OSStatus = noErr
				fillBufList[0].mNumberChannels = realFormat.mChannelsPerFrame
				fillBufList[0].mDataByteSize = kSrcBufSize
				fillBufList[0].mData = tmpMutDat.mutableBytes
				
				// client format is always linear PCM - so here we determine how many frames of lpcm
				// we can read/write given our buffer size
				var numFrames = (kSrcBufSize / realFormat.mBytesPerFrame);
				
				// printf("test %d\n", numFrames);
				
				err = ExtAudioFileRead (fileRef, &numFrames, fillBufList.unsafeMutablePointer);
				//XThrowIfError (err, "ExtAudioFileRead");
				if numFrames == 0 {
					// this is our termination condition
					free(fillBufList.unsafeMutablePointer)
					break;
				}
				
				tmpMutDat.length = Int(numFrames * realFormat.mBytesPerFrame)
				mutableData.appendData(tmpMutDat)
				free(fillBufList.unsafeMutablePointer)
			} else {
				return .NeedMemory
			}
		}
		
		sample.volume = 64
		sample.c2spd = UInt16(realFormat.mSampleRate)
		sample.loopType = .Classic
		sample.relativeNote = 0
		sample.amplitude = MADByte(realFormat.mBitsPerChannel)
		sample.stereo = realFormat.mChannelsPerFrame == 2
		sample.data = mutableData
		
		return .NoErr
	} else {
		return .NeedMemory
	}
}

