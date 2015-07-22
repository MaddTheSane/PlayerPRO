//
//  AIFFSamplePlug.swift
//  PPMacho
//
//  Created by C.W. Betts on 2/11/15.
//
//

import Cocoa
import AudioToolbox
import PlayerPROCore
import PlayerPROKit
import SwiftAdditions
import SwiftAudioAdditions

public final class AIFF: NSObject, PPSampleExportPlugin, PPSampleImportPlugin {
	public let hasUIForExport = false
	
	override init() {
		super.init()
	}
	
	required convenience public init!(forPlugIn: ()) {
		self.init()
	}
	
	public func exportSample(sample: PPSampleObject, toURL sampleURL: NSURL, driver: PPDriver) -> MADErr {
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(sample.c2spd), formatID: .LinearPCM, formatFlags: .SignedInteger | .Packed | .BigEndian, bitsPerChannel: UInt32(sample.amplitude), channelsPerFrame: sample.stereo ? 2 : 1)
		
		let datLen = sample.data.length
		var audioFile: AudioFileID = nil
		var myErr = MADErr.NoErr
		
		var res = AudioFileCreate(URL: sampleURL, fileType: .AIFF, format: &asbd, flags: .EraseFile, audioFile: &audioFile)
		
		if res != noErr {
			return .WritingErr
		} else {
			var numBytes = UInt32(datLen)
			
			if sample.amplitude == 16 {
				if isBigEndian {
					res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, sample.data.bytes);
				} else {
					let maxWriteBytes = 16
					var remaining = datLen
					var location = 0
					var toWriteSize = 16
					var toWriteBytes = [Int16](count: 8, repeatedValue: 0)
					let tmpData = UnsafePointer<Int16>(sample.data.bytes)
					do {
						for i in 0..<(toWriteSize / 2) {
							toWriteBytes[i] = tmpData[location / 2 + i].bigEndian
						}
						remaining -= toWriteSize
						var tmpToWrite = UInt32(toWriteSize)
						res = AudioFileWriteBytes(audioFile, true, Int64(location), &tmpToWrite, toWriteBytes)
						location += toWriteSize
						toWriteSize = min(remaining, maxWriteBytes)
					} while toWriteSize != 0 && res == noErr
					if res != noErr {
						AudioFileClose(audioFile)
						return .WritingErr
					}
				}
			} else {
				res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, sample.data.bytes);
			}
			if (res != noErr) {
				myErr = .WritingErr;
			}
			
			res = AudioFileClose(audioFile);
			if (res != noErr) {
				return .WritingErr
			}
		}
		
		return myErr
	}
	
	//MARK: import
	
	public let hasUIForImport = false
	
	public func canImportSampleAtURL(AlienFileURL: NSURL) -> Bool {
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
	
	public func importSampleAtURL(sampleURL: NSURL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver) -> MADErr {
		let aSamp = PPSampleObject()
		let iErr = AIFFAtURL(sampleURL, toSample: aSamp)
		if iErr == .NoErr {
			aSamp.name = sampleURL.lastPathComponent!.stringByDeletingPathExtension
			sample.memory = aSamp
		}
		
		return iErr
	}
}
