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
		
		res = AudioFileOpen(URL: sampleURL, permissions: .readPermission, fileTypeHint: .WAVE, audioFile: &audioFile);
		if (res != noErr) {
			myErr = .fileNotSupportedByThisPlug;
		} else {
			AudioFileClose(audioFile!);
		}
		
		return myErr == .noErr
	}
	
	public func importSample(at url: URL, sample asample: AutoreleasingUnsafeMutablePointer<PPSampleObject>?, driver: PPDriver) -> MADErr {
		let newSample = PPSampleObject()
		var fileRef1: ExtAudioFileRef? = nil
		var iErr = ExtAudioFileOpenURL(url, &fileRef1)
		if iErr != noErr {
			return .readingErr
		}
		guard let fileRef = fileRef1 else {
			return .readingErr
		}
		defer {
			ExtAudioFileDispose(fileRef)
		}

		if let mutableData = NSMutableData(capacity: Int(kSrcBufSize) * 8) {
			var realFormat = AudioStreamBasicDescription()
			
			var asbdSize = UInt32(sizeof(AudioStreamBasicDescription))
			iErr = ExtAudioFileGetProperty(inExtAudioFile: fileRef, propertyID: kExtAudioFileProperty_FileDataFormat, propertyDataSize: &asbdSize, propertyData: &realFormat)
			if iErr != noErr {
				return .unknownErr
			}
			
			//Constrain the audio conversion to values supported by PlayerPRO
			realFormat.mSampleRate = ceil(realFormat.mSampleRate)
			realFormat.mSampleRate = clamp(value: realFormat.mSampleRate, minimum: 5000, maximum: 44100)
			realFormat.formatFlags = [.NativeEndian, .Packed, .SignedInteger]
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
			
			iErr = ExtAudioFileSetProperty(inExtAudioFile: fileRef, propertyID: kExtAudioFileProperty_ClientDataFormat, dataSize: sizeof(AudioStreamBasicDescription), data: &realFormat)
			if iErr != noErr {
				return .unknownErr
			}

			while true {
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
					var numFrames = (kSrcBufSize / realFormat.mBytesPerFrame);
					
					// printf("test %d\n", numFrames);
					
					err = ExtAudioFileRead(fileRef, &numFrames, fillBufList.unsafeMutablePointer);
					if err != noErr {
						return .unknownErr
					}
					if numFrames == 0 {
						// this is our termination condition
						break;
					}
					
					tmpMutDat.length = Int(numFrames * realFormat.mBytesPerFrame)
					mutableData.append(tmpMutDat as Data)
				} else {
					return .needMemory
				}
			}
			
			newSample.volume = 64
			newSample.c2spd = UInt16(realFormat.mSampleRate)
			newSample.loopType = .classic
			newSample.relativeNote = 0
			newSample.amplitude = MADByte(realFormat.mBitsPerChannel)
			newSample.isStereo = realFormat.mChannelsPerFrame == 2
			newSample.data = mutableData as Data
			
			asample?.pointee = newSample
			
			return .noErr
		} else {
			return .needMemory
		}
	}
	
	public func exportSample(_ sample: PPSampleObject, to sampleURL: URL, driver: PPDriver) -> MADErr {
		var myErr = MADErr.noErr
		var asbd = AudioStreamBasicDescription(sampleRate: Float64(sample.c2spd), formatID: .LinearPCM, formatFlags: [.SignedInteger, .Packed], bitsPerChannel: UInt32(sample.amplitude), channelsPerFrame: sample.isStereo ? 2 : 1)
		
		var audioFile: AudioFileID? = nil
		var res: OSStatus = 0
		let data: Data
		if isBigEndian {
			if (sample.amplitude == 16) {
				let mutData = NSMutableData(data: sample.data)
				let mutShortBytes = UnsafeMutablePointer<UInt16>(mutData.mutableBytes)
				DispatchQueue.concurrentPerform(iterations: sample.data.count / 2, execute: { (i) -> Void in
					mutShortBytes[i] = mutShortBytes[i].littleEndian
				})
				
				data = mutData as Data
			} else {
				data = sample.data
			}
		} else {
			data = sample.data
		}
		
		res = AudioFileCreate(URL: sampleURL, fileType: .WAVE, format: &asbd, flags: .eraseFile, audioFile: &audioFile)
		if (res != noErr) {
			myErr = .writingErr;
		} else {
			var numBytes = UInt32(data.count);
			
			res = AudioFileWriteBytes(audioFile!, false, 0, &numBytes, (data as NSData).bytes);
			if (res != noErr) {
				myErr = .writingErr;
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
				
				res = AudioFileClose(audioFile!);
				if (res != noErr) {
					myErr = .writingErr;
				}
			}
		}
		
		return myErr
	}
}
