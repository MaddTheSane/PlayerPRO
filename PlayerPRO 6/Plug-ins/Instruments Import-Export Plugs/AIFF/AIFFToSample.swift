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

internal func readAIFF(at url: URL) throws -> PPSampleObject {
	guard let fileRef = try? ExtAudioFile(open: url) else {
		throw MADErr.readingErr
	}
	
	let realFormat = fileRef.fileDataFormat
	
	fileRef.clientDataFormat = {
		let sampRate = clamp(ceil(realFormat.mSampleRate), minimum: 5000, maximum: 44100)
		let bytesPerSamp: UInt32
		switch realFormat.mBitsPerChannel {
		case 1...8:
			bytesPerSamp = 8
		//case 9...16:
		//	bytesPerSamp = 16
		//case 20, 24:
		default:
			bytesPerSamp = 16
		}
		
		let chanPerFr: UInt32
		switch realFormat.mChannelsPerFrame {
		case 1, 2:
			chanPerFr = realFormat.mChannelsPerFrame
			
		default:
			chanPerFr = 2
		}
		return AudioStreamBasicDescription(sampleRate: sampRate, formatID: .linearPCM, formatFlags: [.nativeEndian, .packed, .signedInteger], bitsPerChannel: bytesPerSamp, channelsPerFrame: chanPerFr)
	}()
	
	var mutableData = Data(capacity: Int(kSrcBufSize) * 8)
	readLoop: while true {
		if let tmpMutDat = NSMutableData(length: Int(kSrcBufSize)) {
			let fillBufList = AudioBufferList.allocate(maximumBuffers: 1)
			defer {
				free(fillBufList.unsafeMutablePointer)
			}
			fillBufList[0].mNumberChannels = realFormat.mChannelsPerFrame
			fillBufList[0].mDataByteSize = kSrcBufSize
			fillBufList[0].mData = tmpMutDat.mutableBytes
			
			// client format is always linear PCM - so here we determine how many frames of lpcm
			// we can read/write given our buffer size
			var numFrames = (kSrcBufSize / realFormat.mBytesPerFrame);
			
			// printf("test %d\n", numFrames);
			
			try fileRef.read(frames: &numFrames, data: fillBufList.unsafeMutablePointer)
			
			if numFrames == 0 {
				// this is our termination condition
				break readLoop
			}
			
			tmpMutDat.length = Int(numFrames * realFormat.mBytesPerFrame)
			mutableData.append(tmpMutDat as Data)
		} else {
			throw MADErr.needMemory
		}
	}
	
	let sample = PPSampleObject()
	
	sample.volume = 64
	sample.c2spd = UInt16(realFormat.mSampleRate)
	sample.loopType = .classic
	sample.realNote = 0
	sample.amplitude = MADByte(realFormat.mBitsPerChannel)
	sample.isStereo = realFormat.mChannelsPerFrame == 2
	sample.data = mutableData
	
	return sample
}
