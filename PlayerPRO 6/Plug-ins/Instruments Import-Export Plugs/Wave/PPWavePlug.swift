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

private let kSrcBufSize: UInt32 = 32768;

public final class Wave: NSObject, PPSampleImportPlugin, PPSampleExportPlugin {
	public let hasUIForImport = false
	public let hasUIForExport = false
	
	@objc public convenience init!(forPlugIn: Void) {
		self.init()
	}
	
	override public init() {
		super.init()
	}
	
	public func canImportSample(at sampleURL: URL) -> Bool {
		var myErr = MADErr.noErr
		var audioFile: AudioFileID? = nil
		var res: OSStatus = noErr
		
		res = AudioFileOpen(url: sampleURL as NSURL, permissions: .readPermission, fileTypeHint: .WAVE, audioFile: &audioFile);
		if (res != noErr) {
			myErr = .fileNotSupportedByThisPlug;
		} else {
			AudioFileClose(audioFile!);
		}
		
		return myErr == .noErr
	}
	
	public func importSample(at url: URL, sample asample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver) throws {
		let newSample = PPSampleObject()
		var fileRef1: ExtAudioFileRef? = nil
		var iErr = ExtAudioFileOpenURL(url as NSURL, &fileRef1)
		guard iErr == noErr, let fileRef = fileRef1 else {
			throw MADErr.readingErr
		}
		defer {
			ExtAudioFileDispose(fileRef)
		}

		if var mutableData = NSMutableData(capacity: Int(kSrcBufSize) * 8) as NSData? as Data? {
			var realFormat = AudioStreamBasicDescription()
			
			var asbdSize = UInt32(MemoryLayout<AudioStreamBasicDescription>.size)
			iErr = ExtAudioFileGetProperty(fileRef, propertyID: kExtAudioFileProperty_FileDataFormat, propertyDataSize: &asbdSize, propertyData: &realFormat)
			if iErr != noErr {
				throw NSError(domain: NSOSStatusErrorDomain, code: Int(iErr), userInfo: nil)
			}
			
			//Constrain the audio conversion to values supported by PlayerPRO
			realFormat.mSampleRate = ceil(realFormat.mSampleRate)
			realFormat.mSampleRate = clamp(realFormat.mSampleRate, minimum: 5000, maximum: 44100)
			realFormat.formatFlags = [.nativeEndian, .packed, .signedInteger]
			switch realFormat.mBitsPerChannel {
			case 8, 16:
				break
			case 1...7:
				realFormat.mBitsPerChannel = 8
				//case 9...15:
				//	realFormat.mBitsPerChannel = 16
				//case 20, 24:
			default:
				realFormat.mBitsPerChannel = 16
			}
			
			switch realFormat.mChannelsPerFrame {
			case 1, 2:
				break
			default:
				realFormat.mChannelsPerFrame = 2
			}
			
			iErr = ExtAudioFileSetProperty(fileRef, propertyID: kExtAudioFileProperty_ClientDataFormat, dataSize: MemoryLayout<AudioStreamBasicDescription>.size, data: &realFormat)
			if iErr != noErr {
				throw NSError(domain: NSOSStatusErrorDomain, code: Int(iErr), userInfo: nil)
			}

			readLoop: while true {
				if let tmpMutDat = NSMutableData(length: Int(kSrcBufSize)) {
					let fillBufList = AudioBufferList.allocate(maximumBuffers: 1)
					defer {
						free(fillBufList.unsafeMutablePointer)
					}
					var err: OSStatus = noErr
					fillBufList[0].mNumberChannels = realFormat.mChannelsPerFrame
					fillBufList[0].mDataByteSize = kSrcBufSize
					fillBufList[0].mData = tmpMutDat.mutableBytes
					
					// client format is always linear PCM - so here we determine how many frames of lpcm
					// we can read/write given our buffer size
					var numFrames: UInt32 = (kSrcBufSize / realFormat.mBytesPerFrame);
					
					// printf("test %d\n", numFrames);
					
					err = ExtAudioFileRead(fileRef, &numFrames, fillBufList.unsafeMutablePointer);
					if err != noErr {
						throw NSError(domain: NSOSStatusErrorDomain, code: Int(iErr), userInfo: nil)
					}
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
			
			newSample.volume = 64
			newSample.c2spd = UInt16(realFormat.mSampleRate)
			newSample.loopType = .classic
			newSample.relativeNote = 0
			newSample.amplitude = MADByte(realFormat.mBitsPerChannel)
			newSample.isStereo = realFormat.mChannelsPerFrame == 2
			newSample.data = mutableData
			
			asample.pointee = newSample
		} else {
			throw MADErr.needMemory
		}
	}
	
	public func exportSample(_ sample: PPSampleObject, to sampleURL: URL, driver: PPDriver) throws {
		func applyMetadata(to audFile: ExtAudioFile) {
			// TODO: implement!
		}
		let numChannels: UInt32 = sample.isStereo ? 2 : 1
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(sample.c2spd), formatID: .linearPCM, formatFlags: [.signedInteger, .packed], bitsPerChannel: UInt32(sample.amplitude), channelsPerFrame: numChannels)
		let realFormat = AudioStreamBasicDescription(sampleRate: Float64(sample.c2spd), formatID: .linearPCM, formatFlags: [.signedInteger, .packed, .nativeEndian], bitsPerChannel: UInt32(sample.amplitude), channelsPerFrame: numChannels)
		
		let audOut = try ExtAudioFile(createURL: sampleURL, fileType: .WAVE, streamDescription: &asbd, flags: [.eraseFile])
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
	}
}
