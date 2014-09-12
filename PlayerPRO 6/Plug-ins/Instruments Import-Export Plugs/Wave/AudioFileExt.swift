//
//  AudioFileExt.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/28/14.
//
//

import Foundation
import AudioToolbox

extension AudioStreamBasicDescription {
	internal init() {
		mSampleRate = 0
		mFormatID = 0
		mFormatFlags = 0
		mBytesPerPacket = 0
		mFramesPerPacket = 1
		mBytesPerFrame = 1
		mChannelsPerFrame = 1
		mBitsPerChannel = 1
		mReserved = 0
	}
	
	internal init(sampleRate: Float64, formatID: Int, formatFlags: Int, bitsPerChannel: UInt32, channelsPerFrame: UInt32, framesPerPacket: UInt32 = 1) {
		mSampleRate = sampleRate
		mFormatID = UInt32(formatID)
		mFormatFlags = UInt32(formatFlags)
		mBitsPerChannel = bitsPerChannel
		mChannelsPerFrame = channelsPerFrame
		mFramesPerPacket = framesPerPacket
		mBytesPerFrame = mBitsPerChannel * mChannelsPerFrame / 8
		mBytesPerPacket = mBytesPerFrame * mFramesPerPacket
		mReserved = 0
	}
}
