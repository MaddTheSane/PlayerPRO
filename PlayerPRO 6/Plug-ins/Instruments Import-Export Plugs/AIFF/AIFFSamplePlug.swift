//
//  AIFFSamplePlug.swift
//  PPMacho
//
//  Created by C.W. Betts on 2/11/15.
//
//

import Cocoa
import AudioToolbox
import PlayerPROKit
import SwiftAdditions

public final class AIFFSamplePlug: NSObject, PPSampleExportPlugin {
	public let hasUIForExport = false
	
	override init() {
		super.init()
	}
	
	required convenience public init!(forPlugIn: ()) {
		self.init()
	}
	
	public func exportSample(sample: PPSampleObject!, toURL sampleURL: NSURL!, driver: PPDriver!) -> MADErr {
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(sample.c2spd), formatID: .LinearPCM, formatFlags: .SignedInteger | .Packed | .BigEndian, bitsPerChannel: UInt32(sample.amplitude), channelsPerFrame: sample.stereo ? 2 : 1)
		
		let datLen = sample.data.length
		
		var data: UnsafePointer<Int8>
		
		var audioFile: AudioFileID = nil
		var res: OSStatus = noErr
		var myErr = MADErr.NoErr
		
		if sample.amplitude == 16 {
			var tmpData = UnsafeMutablePointer<Int16>(malloc(UInt(datLen)))
			if tmpData == nil {
				return .NeedMemory
			}
			memcpy(tmpData, sample.data.bytes, UInt(datLen))
			dispatch_apply(UInt(datLen) / 2, dispatch_get_global_queue(0, 0), { (i) -> Void in
				let ii = Int(i)
				tmpData[ii] = tmpData[ii].bigEndian
			})
			data = UnsafePointer<Int8>(tmpData)
		} else {
			data = UnsafePointer<Int8>(sample.data.bytes)
		}
		
		res = AudioFileCreate(URL: sampleURL, fileType: .AIFF, format: &asbd, flags: .EraseFile, audioFile: &audioFile)
		
		if res != noErr {
			myErr = .WritingErr
		} else {
			var numBytes = UInt32(datLen)
			
			res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, data);
			if (res != noErr) {
				myErr = .WritingErr;
			}
			
			res = AudioFileClose(audioFile);
			if (res != noErr) {
				myErr = .WritingErr;
			}
		}
		
		if (sample.amplitude == 16) {
			free(UnsafeMutablePointer<Void>(data))
		}
		
		return myErr
	}
	
	//MARK: import
	//TODO: complete
	public let hasUIForImport = false
	
	public func canImportSampleAtURL(AlienFileURL: NSURL!) -> Bool {
		var myErr = MADErr.NoErr
		var audioFile: AudioFileID = nil
		var res: OSStatus = noErr
		
		res = AudioFileOpen(URL: AlienFileURL, permissions: 0x01, fileTypeHint: .AIFF, audioFile: &audioFile);
		if (res != noErr) {
			res = AudioFileOpen(URL: AlienFileURL, permissions: 0x01, fileTypeHint: .AIFC, audioFile: &audioFile);
			if (res != noErr) {
				myErr = .FileNotSupportedByThisPlug;
			} else {
				AudioFileClose(audioFile);
			}
		} else {
			AudioFileClose(audioFile);
		}
		
		return myErr == .NoErr
	}
}
