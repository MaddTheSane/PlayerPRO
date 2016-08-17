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


internal func AIFFAtURL(_ url: URL, toSample sample: PPSampleObject) -> MADErr {
	var fileRef1: ExtAudioFileRef? = nil
	var iErr = ExtAudioFileOpenURL(url as CFURL, &fileRef1)
	if iErr != noErr {
		return .readingErr
	}
	guard let fileRef = fileRef1 else {
		return .readingErr
	}
	defer {
		ExtAudioFileDispose(fileRef)
	}
	if let mutableData = NSMutableData(capacity: Int(kSrcBufSize) * 8) {
		var realFormat = AudioStreamBasicDescription()
		
		var asbdSize = UInt32(MemoryLayout<AudioStreamBasicDescription>.size)
		iErr = ExtAudioFileGetProperty(inExtAudioFile: fileRef, propertyID: kExtAudioFileProperty_FileDataFormat, propertyDataSize: &asbdSize, propertyData: &realFormat)
		if iErr != noErr {
			return .unknownErr
		}
		
		//Constrain the audio conversion to values supported by PlayerPRO
		realFormat.mSampleRate = ceil(realFormat.mSampleRate)
		realFormat.mSampleRate = clamp(value: realFormat.mSampleRate, minimum: 5000, maximum: 44100)
		realFormat.formatFlags = [.NativeEndian, .Packed, .SignedInteger]
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
		
		iErr = ExtAudioFileSetProperty(inExtAudioFile: fileRef, propertyID: kExtAudioFileProperty_ClientDataFormat, dataSize: MemoryLayout<AudioStreamBasicDescription>.size, data: &realFormat)
		if iErr != noErr {
			return .unknownErr
		}
		
		while true {
			if let tmpMutDat = NSMutableData(length: Int(kSrcBufSize)) {
				let fillBufList = AudioBufferList.allocate(maximumBuffers: 1)
				defer {
					free(fillBufList.unsafeMutablePointer)
				}
				var err: OSStatus = noErr
				fillBufList[0].mNumberChannels = realFormat.mChannelsPerFrame
				fillBufList[0].mDataByteSize = kSrcBufSize
				fillBufList[0].mData = tmpMutDat.mutableBytes
				
				// client format is always linear PCM - so here we determine how many frames of lpcm
				// we can read/write given our buffer size
				var numFrames = (kSrcBufSize / realFormat.mBytesPerFrame);
				
				// printf("test %d\n", numFrames);
				
				err = ExtAudioFileRead(fileRef, &numFrames, fillBufList.unsafeMutablePointer);
				if err != noErr {
					return .unknownErr
				}
				//XThrowIfError (err, "ExtAudioFileRead");
				if numFrames == 0 {
					// this is our termination condition
					break;
				}
				
				tmpMutDat.length = Int(numFrames * realFormat.mBytesPerFrame)
				mutableData.append(tmpMutDat as Data)
			} else {
				return .needMemory
			}
		}
		
		sample.volume = 64
		sample.c2spd = UInt16(realFormat.mSampleRate)
		sample.loopType = .classic
		sample.relativeNote = 0
		sample.amplitude = MADByte(realFormat.mBitsPerChannel)
		sample.isStereo = realFormat.mChannelsPerFrame == 2
		sample.data = mutableData as Data
		
		return .noErr
	} else {
		return .needMemory
	}
}
