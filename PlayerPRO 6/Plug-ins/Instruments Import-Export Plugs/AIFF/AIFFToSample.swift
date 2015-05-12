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


internal func AIFFAtURL(url: NSURL, toSample sample: PPSampleObject) -> Bool {
	var fileRef: ExtAudioFileRef = nil
	var iErr = ExtAudioFileOpenURL(url, &fileRef)
	if iErr != noErr {
		return false
	}
	let mutableData = NSMutableData()
	
	var realFormat = AudioStreamBasicDescription()
	
	var asbdSize = UInt32(sizeof(AudioStreamBasicDescription))
	iErr = ExtAudioFileGetProperty(fileRef, propertyID: .FileDataFormat, propertyDataSize: &asbdSize, propertyData: &realFormat)
	realFormat.mSampleRate = ceil(realFormat.mSampleRate)
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
	
	iErr = ExtAudioFileSetProperty(fileRef, propertyID: .ClientDataFormat, dataSize: UInt32(sizeof(AudioStreamBasicDescription)), data: &realFormat)
	
	//sample.name
	
	var bufferCount: UInt32 = 0
	var bufferList = AudioBufferList.allocate(maximumBuffers: 5)
	//bufferList.count
	//iErr = ExtAudioFileRead(fileRef, &bufferCount, bufferList.unsafeMutablePointer)
	free(bufferList.unsafeMutablePointer)
	ExtAudioFileDispose(fileRef)
	
	sample.volume = 64
	sample.c2spd = UInt16(realFormat.mSampleRate)
	sample.loopType = .Classic
	sample.relativeNote = 0
	sample.amplitude = MADByte(realFormat.mBitsPerChannel)
	sample.stereo = realFormat.mChannelsPerFrame == 2

	return false
}

