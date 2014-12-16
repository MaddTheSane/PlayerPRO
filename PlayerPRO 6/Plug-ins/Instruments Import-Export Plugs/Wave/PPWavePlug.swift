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

@objc(PPWavePlug) public final class PPWavePlug: NSObject, PPInstrumentImportPlugin, PPInstrumentExportPlugin {
	public let hasUIConfiguration = false
	public let isInstrument = false
	
	public convenience init(forPlugIn: ()) {
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
	
	public func importSampleAtURL(sampleURL: NSURL!, instrument InsHeader: PPInstrumentObject!, sample: PPSampleObject!, sampleID: UnsafeMutablePointer<Int16>, driver: PPDriver!) -> MADErr {
		var soundSize: UInt = 0;
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
		sample.data = NSData(bytesNoCopy: sndPtr, length: Int(soundSize), freeWhenDone: true)
		
		if sampleID.memory == -1 {
			InsHeader.addSampleObject(sample)
			sampleID.memory = Int16(InsHeader.countOfSamples - 1)
		} else {
			InsHeader.replaceObjectInSamplesAtIndex(Int(sampleID.memory), withObject: sample)
		}
		
		return .NoErr
	}
	
	public func exportSampleToURL(sampleURL: NSURL!, instrument InsHeader: PPInstrumentObject!, sample: PPSampleObject!, sampleID: Int16, driver: PPDriver!) -> MADErr {
		var myErr = MADErr.NoErr
		if (sampleID >= 0) {
			var curData = InsHeader.samplesObjectAtIndex(Int(sampleID))
			var asbd = AudioStreamBasicDescription(sampleRate: Float64(curData.c2spd), formatID: .LinearPCM, formatFlags: .SignedInteger | .Packed, bitsPerChannel: UInt32(curData.amplitude), channelsPerFrame: curData.stereo ? 2 : 1)
			
			var audioFile: AudioFileID = nil
			var res: OSStatus = 0
			var data: NSData
			if isBigEndian {
				if (curData.amplitude == 16) {
					var mutData = curData.data.mutableCopy() as NSMutableData
					var mutShortBytes = UnsafeMutablePointer<UInt16>(mutData.mutableBytes)
					dispatch_apply(UInt(curData.data.length / 2), dispatch_get_global_queue(0, 0), { (i) -> Void in
						let IntI = Int(i)
						mutShortBytes[IntI] = mutShortBytes[IntI].littleEndian
						return
					})
					
					data = mutData
				} else {
					data = curData.data
				}
			} else {
				data = curData.data
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
		}
		return myErr
	}
}
