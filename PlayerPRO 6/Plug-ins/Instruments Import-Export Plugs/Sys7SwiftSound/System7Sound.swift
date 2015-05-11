//
//  System7Sound.swift
//  PPMacho
//
//  Created by C.W. Betts on 5/11/15.
//
//

import Cocoa
import AudioToolbox
import PlayerPROCore
import PlayerPROKit
import SwiftAdditions
import SwiftAudioAdditions

private func fixedToFloat(a: UInt32) -> Double {
	let fixed1 = Double(0x00010000)
	
	return Double(a) / fixed1
}

public final class System7Sound: NSObject, PPSampleImportPlugin {
	public let hasUIForImport = false

	override init() {
		super.init()
	}
	
	required convenience public init!(forPlugIn: ()) {
		self.init()
	}

	public func canImportSampleAtURL(AlienFileURL: NSURL) -> Bool {
		return false
	}
	
	func assetForSND(data: NSData, inout errmsg: String) -> NSURL? {
		// See Sound.h in Carbon
		// Also see "Sound Manager" legacy PDF
		let firstSoundFormat: Int16  = 0x0001 /*general sound format*/
		let secondSoundFormat: Int16 = 0x0002 /*special sampled sound format (HyperCard)*/
		let initMono:   Int32 = 0x0080 /*monophonic channel*/
		let initStereo: Int32 = 0x00C0 /*stereo channel*/
		let initMACE3:  Int32 = 0x0300 /*MACE 3:1*/
		let initMACE6:  Int32 = 0x0400 /*MACE 6:1*/
		let nullCmd: UInt16   = 0
		let soundCmd: UInt16  = 80
		let bufferCmd: UInt16 = 81
		let stdSH: UInt8 = 0x00 /*Standard sound header encode value*/
		let extSH: UInt8 = 0xFF /*Extended sound header encode value*/
		let cmpSH: UInt8 = 0xFE /*Compressed sound header encode value*/
		struct ModRef {
			var modNumber: UInt16 = 0
			var modInit: Int32 = 0
		}
		struct SndCommand {
			var cmd: UInt16 = 0
			var param1: Int16 = 0
			var param2: Int32 = 0
		}
		struct SndListResource {
			var format: Int16 = 0
			var numModifiers: Int16 = 0
			var modifierPart = ModRef()
			var numCommands: Int16 = 0
			var commandPart = SndCommand()
		}
		struct Snd2ListResource {
			var format: Int16 = 0
			var refCount: Int16 = 0
			var numCommands: Int16 = 0
			var commandPart = SndCommand()
		}
		struct SoundHeader {
			var samplePtr: UInt32 = 0
			var length: UInt32 = 0
			var sampleRate: UInt32 = 0
			var loopStart: UInt32 = 0
			var loopEnd: UInt32 = 0
			var encode: UInt8 = 0
			var baseFrequency: UInt8 = 0
		}
		
		let reader = FVDataReader(data)
		
		// Read the SndListResource or Snd2ListResource
		var format = Int16()
		if !reader.readInt16(.Big, &format) {
			errmsg = "Missing header"
			return nil
		}
		if format == firstSoundFormat {
			var numModifiers = Int16()
			var modifierPart = ModRef()
			if !reader.readInt16(.Big, &numModifiers) ||
				!reader.readUInt16(.Big, &modifierPart.modNumber) ||
				!reader.readInt32(.Big, &modifierPart.modInit) {
					errmsg = "Missing header"
					return nil
			}
			if numModifiers != 1 {
				errmsg = "Bad header"
				return nil
			}
			if modifierPart.modNumber != 5  {
				errmsg = "Unknown modNumber value \(modifierPart.modNumber)"
				return nil
			}
			if modifierPart.modInit & initStereo == 1 {
				errmsg = "Only mono channel supported"
				return nil
			}
			if modifierPart.modInit & initMACE3 == 1 || modifierPart.modInit & initMACE6 == 1 {
				errmsg = "Compression not supported"
				return nil
			}
		} else if format == secondSoundFormat {
			var refCount = Int16()
			if !reader.readInt16(.Big, &refCount) {
				errmsg = "Missing header"
				return nil
			}
		} else {
			errmsg = "Unknown format \(format)"
			return nil
		}
		
		// Read SndCommands
		var header_offset = Int()
		var numCommands = Int16()
		var commandPart = SndCommand()
		if !reader.readInt16(.Big, &numCommands) {
			errmsg = "Missing header"
			return nil
		}
		if numCommands == 0 {
			errmsg = "Bad header"
			return nil
		}
		for var i = Int16(0); i < numCommands; ++i {
			if !reader.readUInt16(.Big, &commandPart.cmd) ||
				!reader.readInt16(.Big, &commandPart.param1) ||
				!reader.readInt32(.Big, &commandPart.param2) {
					errmsg = "Missing command"
					return nil
			}
			// "If soundCmd is contained within an 'snd ' resource, the high bit of the command must be set."
			// Apple docs says this for bufferCmd as well, so we clear the bit.
			commandPart.cmd &= ~0x8000
			switch commandPart.cmd {
			case soundCmd, bufferCmd:
				if header_offset != 0 {
					errmsg = "Duplicate commands"
					return nil
				}
				header_offset = Int(commandPart.param2)
			case nullCmd:
				break
			default:
				errmsg = "Unknown command \(commandPart.cmd)"
				return nil
			}
		}
		
		// Read SoundHeader
		var header = SoundHeader()
		if !reader.readUInt32(.Big, &header.samplePtr) ||
			!reader.readUInt32(.Big, &header.length) ||
			!reader.readUInt32(.Big, &header.sampleRate) ||
			!reader.readUInt32(.Big, &header.loopStart) ||
			!reader.readUInt32(.Big, &header.loopEnd) ||
			!reader.readUInt8(&header.encode) ||
			!reader.readUInt8(&header.baseFrequency) {
				errmsg = "Missing header data"
				return nil
		}
		let sampleData = reader.read(Int(header.length))
		if sampleData == nil {
			errmsg = "Missing samples"
			return nil
		}
		if header.encode != stdSH {
			if header.encode == extSH {
				errmsg = "Extended encoding not supported"
			} else if header.encode == cmpSH {
				errmsg = "Compression not supported"
			} else {
				errmsg = String(format: "Unknown encoding 0x%02X", header.encode)
			}
			return nil
		}
		
		// Generate an AudioStreamBasicDescription for conversion
		var stream = AudioStreamBasicDescription()
		stream.mSampleRate = fixedToFloat(header.sampleRate)
		stream.mFormatID = AudioFormatID(kAudioFormatLinearPCM)
		stream.mFormatFlags = AudioFormatFlags(kLinearPCMFormatFlagIsSignedInteger)
		stream.mBytesPerPacket = 1
		stream.mFramesPerPacket = 1
		stream.mBytesPerFrame = 1
		stream.mChannelsPerFrame = 1
		stream.mBitsPerChannel = 8
		
		// Create a temporary file for storage
		if let url = NSURL(fileURLWithPath: NSTemporaryDirectory().stringByAppendingFormat("%d-%f.aif", arc4random(), NSDate().timeIntervalSinceReferenceDate)) {
			var audioFile: ExtAudioFileRef = nil
			let createStatus = ExtAudioFileCreateWithURL(url, AudioFileTypeID(kAudioFileAIFFType), &stream, nil, UInt32(kAudioFileFlags_EraseFile), &audioFile)
			if createStatus != noErr {
				errmsg = "ExtAudioFileCreateWithURL failed with status \(createStatus)"
				return nil
			}
			
			// Configure the AudioBufferList
			let srcData = UnsafePointer<UInt8>(sampleData!.bytes)
			var audioBuffer = AudioBuffer()
			audioBuffer.mNumberChannels = 1
			audioBuffer.mDataByteSize = header.length
			audioBuffer.mData = UnsafeMutablePointer(srcData)
			var audioBufferData = UnsafeMutablePointer<UInt8>(audioBuffer.mData)
			for var i = 0; i < Int(header.length); ++i {
				audioBufferData[i] ^= 0x80
			}
			var bufferList = AudioBufferList(mNumberBuffers: 1, mBuffers: audioBuffer)
			
			// Write the data to the file
			let writeStatus = ExtAudioFileWrite(audioFile, header.length, &bufferList)
			if writeStatus != noErr {
				errmsg = "ExtAudioFileWrite failed with status \(writeStatus)"
				return nil
			}
			
			// Finish up
			let disposeStatus = ExtAudioFileDispose(audioFile)
			if disposeStatus != noErr {
				errmsg = "ExtAudioFileDispose failed with status \(disposeStatus)"
				return nil
			}
			
			// Generate an AVAsset
			return url
		} else {
			errmsg = "Can't make url for conversion"
			return nil
		}
	}
	
	public func importSampleAtURL(sampleURL: NSURL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver) -> MADErr {
		return .OrderNotImplemented
	}
}
