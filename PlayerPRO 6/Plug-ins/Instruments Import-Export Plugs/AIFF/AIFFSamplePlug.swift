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
		func applyMetadata(to audFile: ExtAudioFile) {
			// TODO: implement!
		}
		let numChannels: UInt32 = sample.isStereo ? 2 : 1
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(sample.c2spd), formatID: .linearPCM, formatFlags: [AudioFormatFlag.signedInteger, .packed, .bigEndian], bitsPerChannel: UInt32(sample.amplitude), channelsPerFrame: numChannels)
		let realFormat = AudioStreamBasicDescription(sampleRate: Float64(sample.c2spd), formatID: .linearPCM, formatFlags: [AudioFormatFlag.signedInteger, .packed, .nativeEndian], bitsPerChannel: UInt32(sample.amplitude), channelsPerFrame: numChannels)

		do {
			let audOut = try ExtAudioFile(createURL: sampleURL, fileType: .AIFF, streamDescription: &asbd, flags: [.eraseFile])
			audOut.clientDataFormat = realFormat

			try sample.data.withUnsafeBytes { (toWriteBytes: UnsafePointer<UInt8>) -> Void in
				let toWriteSize = sample.data.count
				
				var audBufList = AudioBufferList()
				audBufList.mNumberBuffers = 1
				audBufList.mBuffers.mNumberChannels = numChannels
				audBufList.mBuffers.mDataByteSize = UInt32(toWriteSize)
				audBufList.mBuffers.mData = UnsafeMutableRawPointer(mutating: toWriteBytes)
				
				try audOut.write(frames: UInt32(toWriteSize) / realFormat.mBytesPerFrame, data: &audBufList)
			}
			
			applyMetadata(to: audOut)
		} catch let error as MADErr {
			return error
		} catch {
			return .writingErr
		}
		
		return .noErr
	}
	
	//MARK: import
	
	public let hasUIForImport = false
	
	public func canImportSample(at AlienFileURL: URL) -> Bool {
		var myErr = MADErr.noErr
		var audioFile: AudioFileID? = nil
		var res: OSStatus = noErr
		
		res = AudioFileOpen(url: AlienFileURL as NSURL, permissions: .readPermission, fileTypeHint: .AIFF, audioFile: &audioFile);
		if res != noErr {
			res = AudioFileOpen(url: AlienFileURL as NSURL, permissions: .readPermission, fileTypeHint: .AIFC, audioFile: &audioFile);
			if res != noErr {
				myErr = .fileNotSupportedByThisPlug;
			} else {
				AudioFileClose(audioFile!);
			}
		} else {
			AudioFileClose(audioFile!);
		}
		
		return myErr == .noErr
	}
	
	public func importSample(at sampleURL: URL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver) -> MADErr {
		do {
			let aSamp = try readAIFF(at: sampleURL)
			aSamp.name = (sampleURL.lastPathComponent as NSString).deletingPathExtension
			sample.pointee = aSamp
			return .noErr
		} catch let error as MADErr {
			return error
		} catch {
			return .unknownErr
		}
	}
}
