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

@objc(PPWavePlug) public class PPWavePlug: NSObject, PPInstrumentImportPlugin, PPInstrumentExportPlugin {
	public var hasUIConfiguration: Bool {
		get {
			return false
		}
	}
	
	public var instrument: Bool {
		get {
			return false
		}
	}
	
	public func canImportSampleAtURL(sampleURL: NSURL!) -> Bool {
		var sampleHandle = NSFileHandle.fileHandleForReadingFromURL(sampleURL, error: nil)
		if sampleHandle == nil {
			return false
		}
		let headerDat = sampleHandle.readDataOfLength(100)
		sampleHandle.closeFile()
		
		var aErr = TestWAV(PCMWavePtr(headerDat.bytes))
		
		return aErr == .NoErr
	}
	
	public func importSampleAtURL(sampleURL: NSURL!, instrument InsHeader: PPInstrumentObject!, sample: PPSampleObject!, sampleID: UnsafeMutablePointer<Int16>, driver: PPDriver!) -> MADErr {
		
		var soundSize: UInt = 0;
		var loopStart:Int32 = 0
		var loopEnd:Int32 = 0
		var sampleSize: Int16 = 0
		var rate: UInt32 = 0
		var stereo = false
		
		let sndPtr = ConvertWAVCFURL(sampleURL as CFURL, &soundSize, &loopStart, &loopEnd, &sampleSize, &rate, &stereo)
		
		var sample = PPSampleObject()
		sample.loopBegin = loopStart
		sample.loopSize = loopEnd - loopStart
		//sample.volume = 64
		sample.amplitude = MADByte(sampleSize)
		sample.c2spd = UInt16(rate)
		sample.relativeNote = 0
		sample.stereo = stereo
		sample.data = NSData(bytesNoCopy: sndPtr, length: Int(soundSize), freeWhenDone: true)
		
		return .UnknownErr
	}
	
	public func exportSampleToURL(sampleURL: NSURL!, instrument InsHeader: PPInstrumentObject!, sample: PPSampleObject!, sampleID: Int16, driver: PPDriver!) -> MADErr {
		var myErr = MADErr.NoErr
		if (sampleID >= 0) {
			var curData = InsHeader.samplesObjectAtIndex(Int(sampleID))
			var asbd = AudioStreamBasicDescription()
			asbd.mSampleRate = Float64(curData.c2spd)
			asbd.mFormatID = AudioFormatID(kAudioFormatLinearPCM)
			asbd.mFormatFlags = AudioFormatFlags(kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked)
			asbd.mBitsPerChannel = UInt32(curData.amplitude)
			asbd.mChannelsPerFrame = curData.stereo ? 2 : 1;
			asbd.mFramesPerPacket = 1;
			asbd.mBytesPerFrame = asbd.mBitsPerChannel * asbd.mChannelsPerFrame / 8;
			asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;
			
			var audioFile: AudioFileID = nil
			var res: OSStatus = 0
			var data: NSData
			#if false
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
				data = curData.data;
			}
			#else
				data = curData.data
			#endif
			
			res = AudioFileCreateWithURL(sampleURL as CFURL, AudioFileTypeID(kAudioFileWAVEType), &asbd, UInt32(kAudioFileFlags_EraseFile), &audioFile);
			if (res != noErr) {
				myErr = .WritingErr;
			} else {
				var numBytes = UInt32(data.length);
				
				res = AudioFileWriteBytes(audioFile, 0, 0, &numBytes, data.bytes);
				if (res != noErr) {
					myErr = .WritingErr;
				} else {
					#if false
						if (curData.loopBegin != 0) {
						var theLoop = AIFFLoop(playMode: 0, beginLoop: 0, endLoop: 0);
						AudioFileSetUserData(audioFile, "LOOP", 0, UInt32(sizeof(AIFFLoop)), &theLoop);
						}
						AudioFileOptimize(audioFile);
					#endif
					
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
