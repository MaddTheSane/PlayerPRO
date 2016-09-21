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
		
	public func exportSample(_ sample: PPSampleObject, to sampleURL: URL, driver: PPDriver) -> MADErr {
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(sample.c2spd), formatID: .linearPCM, formatFlags: [AudioFormatFlag.signedInteger, .packed, .bigEndian], bitsPerChannel: UInt32(sample.amplitude), channelsPerFrame: sample.isStereo ? 2 : 1)
		
		let datLen = sample.data.count
		var audioFile: AudioFileID? = nil
		
		var res = AudioFileCreate(URL: sampleURL, fileType: .AIFF, format: &asbd, flags: .eraseFile, audioFile: &audioFile)
		
		if res != noErr {
			return .writingErr
		} else {
			guard let audioFile = audioFile else {
				return .writingErr
			}
			defer {
				AudioFileClose(audioFile)
			}
			var numBytes = UInt32(datLen)
			
			return sample.data.withUnsafeBytes({ (aData: UnsafePointer<Int8>) -> MADErr in
				var myErr = MADErr.noErr
				if sample.amplitude == 16 {
					if ByteOrder.isBig {
						//sample.data.
						res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, aData);
					} else {
						let maxWriteBytes = 16
						var remaining = datLen
						var location = 0
						var toWriteSize = 16
						var toWriteBytes = [Int16](repeating: 0, count: 8)
						let tmpData = UnsafeRawPointer(aData).assumingMemoryBound(to: Int16.self)
						repeat {
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
							return .writingErr
						}
					}
				} else {
					res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, aData);
				}
				if (res != noErr) {
					myErr = .writingErr;
				}
				return myErr
			})
		}
	}
	
	//MARK: import
	
	public let hasUIForImport = false
	
	public func canImportSample(at AlienFileURL: URL) -> Bool {
		var myErr = MADErr.noErr
		var audioFile: AudioFileID? = nil
		var res: OSStatus = noErr
		
		res = AudioFileOpen(URL: AlienFileURL as NSURL, permissions: .readPermission, fileTypeHint: .AIFF, audioFile: &audioFile);
		if (res != noErr) {
			res = AudioFileOpen(URL: AlienFileURL as NSURL, permissions: .readPermission, fileTypeHint: .AIFC, audioFile: &audioFile);
			if (res != noErr) {
				myErr = .fileNotSupportedByThisPlug;
			} else {
				AudioFileClose(audioFile!);
			}
		} else {
			AudioFileClose(audioFile!);
		}
		
		return myErr == .noErr
	}
	
	public func importSample(at sampleURL: URL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject>?, driver: PPDriver) -> MADErr {
		let aSamp = PPSampleObject()
		let iErr = AIFFAtURL(sampleURL, toSample: aSamp)
		if iErr == .noErr {
			aSamp.name = (sampleURL.lastPathComponent as NSString).deletingPathExtension
			sample!.pointee = aSamp
		}
		
		return iErr
	}
}
