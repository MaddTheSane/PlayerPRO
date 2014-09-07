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
}


