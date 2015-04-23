//
//  PPWavePlug.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/7/14.
//
//

import Cocoa
import PlayerPROKit
import AudioUnit
import AudioToolbox
import SwiftAdditions
import SwiftAudioAdditions

public final class Wave: NSObject, PPSampleImportPlugin, PPSampleExportPlugin {
	public let hasUIForImport = false
	public let hasUIForExport = false
	
	@objc public convenience init!(forPlugIn: Void) {
		self.init()
	}
	
	override public init() {
		super.init()
	}
	
	public func canImportSampleAtURL(sampleURL: NSURL!) -> Bool {
		if let sampleHandle = NSFileHandle(forReadingFromURL: sampleURL, error: nil) {
			let headerDat = sampleHandle.readDataOfLength(sizeof(PCMWaveRec.Type) + 20)
			sampleHandle.closeFile()
			
			let aErr = TestWAV(PCMWavePtr(headerDat.bytes))
			
			return aErr == .NoErr
		} else {
			return false
		}
	}
	
	public func importSampleAtURL(sampleURL: NSURL!, sample asample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver!) -> MADErr {
		var soundSize: Int = 0;
		var loopStart:Int32 = 0
		var loopEnd:Int32 = 0
		var sampleSize: Int16 = 0
		var rate: UInt32 = 0
		var stereo = false
		
		let sndPtr = ConvertWAVCFURL(sampleURL, &soundSize, &loopStart, &loopEnd, &sampleSize, &rate, &stereo)
		
		if sndPtr == nil {
			return .ReadingErr
		}
		
		var sample = PPSampleObject()
		sample.loopBegin = loopStart
		sample.loopSize = loopEnd - loopStart
		sample.volume = 64
		sample.amplitude = MADByte(sampleSize)
		sample.c2spd = UInt16(rate)
		sample.relativeNote = 0
		sample.stereo = stereo
		sample.data = NSData(bytesNoCopy: sndPtr, length: soundSize, freeWhenDone: true)
		
		asample.memory = sample
		
		return .NoErr
	}
	
	public func exportSample(sample: PPSampleObject!, toURL sampleURL: NSURL!, driver: PPDriver!) -> MADErr {
		var myErr = MADErr.NoErr
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(sample.c2spd), formatID: .LinearPCM, formatFlags: .SignedInteger | .Packed, bitsPerChannel: UInt32(sample.amplitude), channelsPerFrame: sample.stereo ? 2 : 1)
		
		var audioFile: AudioFileID = nil
		var res: OSStatus = 0
		var data: NSData
		if isBigEndian {
			if (sample.amplitude == 16) {
				var mutData = NSMutableData(data: sample.data)
				var mutShortBytes = UnsafeMutablePointer<UInt16>(mutData.mutableBytes)
				dispatch_apply(sample.data.length / 2, dispatch_get_global_queue(0, 0), { (i) -> Void in
					let IntI = Int(i)
					mutShortBytes[IntI] = mutShortBytes[IntI].littleEndian
					return
				})
				
				data = mutData
			} else {
				data = sample.data
			}
		} else {
			data = sample.data
		}
		
		res = AudioFileCreate(URL: sampleURL, fileType: .WAVE, format: &asbd, flags: .EraseFile, audioFile: &audioFile)
		if (res != noErr) {
			myErr = .WritingErr;
		} else {
			var numBytes = UInt32(data.length);
			
			res = AudioFileWriteBytes(audioFile, 0, 0, &numBytes, data.bytes);
			if (res != noErr) {
				myErr = .WritingErr;
			} else {
				if sample.loop != .notFound {
					
				}
				/*
				if (curData.loopBegin != 0) {
				var theLoop = AIFFLoop(playMode: 0, beginLoop: 0, endLoop: 0);
				AudioFileSetUserData(audioFile, "LOOP", 0, UInt32(sizeof(AIFFLoop)), &theLoop);
				}
				AudioFileOptimize(audioFile);
				*/
				
				res = AudioFileClose(audioFile);
				if (res != noErr) {
					myErr = .WritingErr;
				}
			}
		}
		
		return myErr
	}
}
