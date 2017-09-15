//
//  sndConverter.swift
//  PPMacho
//
//  Created by C.W. Betts on 5/11/15.
//
//

import Foundation
import AudioToolbox
import SwiftAudioAdditions
import PlayerPROCore.Defines
import ResourceFork

private func fixedToFloat(_ a: UInt32) -> Double {
	let fixed1 = Double(0x00010000)
	
	return Double(a) / fixed1
}

// See Sound.h in Carbon
// Also see "Sound Manager" legacy PDF
private let firstSoundFormat:	Int16 = 0x0001 /*general sound format*/
private let secondSoundFormat:	Int16 = 0x0002 /*special sampled sound format (HyperCard)*/

private let initMono:   Int32 = 0x0080 /*monophonic channel*/
private let initStereo: Int32 = 0x00C0 /*stereo channel*/
private let initMACE3:  Int32 = 0x0300 /*MACE 3:1*/
private let initMACE6:  Int32 = 0x0400 /*MACE 6:1*/

private let nullCmd:	UInt16 = 0
private let soundCmd:	UInt16 = 80
private let bufferCmd:	UInt16 = 81

private let stdSH: UInt8 = 0x00 /*Standard sound header encode value*/
private let extSH: UInt8 = 0xFF /*Extended sound header encode value*/
private let cmpSH: UInt8 = 0xFE /*Compressed sound header encode value*/


private struct ModRef {
	var modNumber: UInt16 = 0
	var modInit: Int32 = 0
}
private struct SndCommand {
	var cmd: UInt16 = 0
	var param1: Int16 = 0
	var param2: Int32 = 0
}
private struct SndListResource {
	var format: Int16 = 0
	var numModifiers: Int16 = 0
	var modifierPart = ModRef()
	var numCommands: Int16 = 0
	var commandPart = SndCommand()
}
private struct Snd2ListResource {
	var format: Int16 = 0
	var refCount: Int16 = 0
	var numCommands: Int16 = 0
	var commandPart = SndCommand()
}
private struct SoundHeader {
	var samplePtr: UInt32 = 0
	var length: UInt32 = 0
	var sampleRate: UInt32 = 0
	var loopStart: UInt32 = 0
	var loopEnd: UInt32 = 0
	var encode: UInt8 = 0
	var baseFrequency: UInt8 = 0
}

internal func canOpenData(_ data: Data) -> Bool {
	#if false
	var soundPtr = UnsafePointer<Int16>(data.bytes)
	let oldSound = soundPtr.memory.bigEndian
	if oldSound == firstSoundFormat || oldSound == secondSoundFormat {
		return true
	} else {
		return false
	}
	#else
		let reader = FVDataReader(data)
		
		// Read the SndListResource or Snd2ListResource
		var format: Int16 = 0
		if !reader.readInt16(endian: .big, &format) {
			return false
		}
		if format == firstSoundFormat {
			var numModifiers: Int16 = 0
			var modifierPart = ModRef()
			if !reader.readInt16(endian: .big, &numModifiers) ||
				!reader.readUInt16(endian: .big, &modifierPart.modNumber) ||
				!reader.readInt32(endian: .big, &modifierPart.modInit) {
					return false
			}
			if numModifiers != 1 {
				return false
			}
			if modifierPart.modNumber != 5  {
				return false
			}
			if modifierPart.modInit & initStereo == 1 {
				return false
			}
			if (modifierPart.modInit & initMACE3) == initMACE3 || (modifierPart.modInit & initMACE6) == initMACE6 {
				return false
			}
		} else if format == secondSoundFormat {
			var refCount = Int16()
			if !reader.readInt16(endian: .big, &refCount) {
				return false
			}
		} else {
			return false
		}
		return true
	#endif
}

enum SndAssetErrors: Error {
	case missingHeader
	case badHeader
	case unknownModNumber(modNum: UInt16)
	case unsupportedConfiguration
	case duplicateCommands
	case missingCommand
	case unknownCommand(cmd: UInt16)
	case unknownEncoding(enc: UInt8)
	case missingSamples
}

internal func assetForSND(_ data: Data) throws -> URL {
	func errmsg( _ message: @autoclosure @escaping  () -> String) {
		print("Sys7 import: \(message())")
	}
	
	let reader = FVDataReader(data)
	
	// Read the SndListResource or Snd2ListResource
	var format = Int16()
	if !reader.readInt16(endian: .big, &format) {
		errmsg("Missing header")
		throw SndAssetErrors.missingHeader
	}
	if format == firstSoundFormat {
		var numModifiers = Int16()
		var modifierPart = ModRef()
		if !reader.readInt16(endian: .big, &numModifiers) ||
			!reader.readUInt16(endian: .big, &modifierPart.modNumber) ||
			!reader.readInt32(endian: .big, &modifierPart.modInit) {
			errmsg("Missing header")
			throw MADErr.fileNotSupportedByThisPlug
		}
		if numModifiers != 1 {
			errmsg("Bad header")
			throw SndAssetErrors.badHeader
		}
		if modifierPart.modNumber != 5  {
			errmsg("Unknown modNumber value \(modifierPart.modNumber)")
			throw SndAssetErrors.unknownModNumber(modNum: modifierPart.modNumber)
		}
		if modifierPart.modInit & initStereo == 1 {
			errmsg("Only mono channel supported")
			throw SndAssetErrors.unsupportedConfiguration
		}
		if (modifierPart.modInit & initMACE3) == initMACE3 || (modifierPart.modInit & initMACE6) == initMACE6 {
			errmsg("Compression not supported")
			throw SndAssetErrors.unsupportedConfiguration
		}
	} else if format == secondSoundFormat {
		var refCount = Int16()
		if !reader.readInt16(endian: .big, &refCount) {
			errmsg("Missing header")
			throw SndAssetErrors.missingHeader
		}
	} else {
		errmsg("Unknown format \(format)")
		throw MADErr.fileNotSupportedByThisPlug
	}
	
	// Read SndCommands
	var header_offset = Int()
	var numCommands = Int16()
	var commandPart = SndCommand()
	if !reader.readInt16(endian: .big, &numCommands) {
		errmsg("Missing header")
		throw SndAssetErrors.missingHeader
	}
	if numCommands == 0 {
		errmsg("Bad header")
		throw SndAssetErrors.badHeader
	}
	for _ in Int16(0) ..< numCommands {
		if !reader.readUInt16(endian: .big, &commandPart.cmd) ||
			!reader.readInt16(endian: .big, &commandPart.param1) ||
			!reader.readInt32(endian: .big, &commandPart.param2) {
			throw SndAssetErrors.missingCommand
		}
		// "If soundCmd is contained within an 'snd ' resource, the high bit of the command must be set."
		// Apple docs says this for bufferCmd as well, so we clear the bit.
		commandPart.cmd &= ~0x8000
		switch commandPart.cmd {
		case soundCmd, bufferCmd:
			if header_offset != 0 {
				errmsg("Duplicate commands")
				throw SndAssetErrors.duplicateCommands
			}
			header_offset = Int(commandPart.param2)
		case nullCmd:
			break
		default:
			errmsg("Unknown command \(commandPart.cmd)")
			throw SndAssetErrors.unknownCommand(cmd: commandPart.cmd)
		}
	}
	
	// Read SoundHeader
	var header = SoundHeader()
	if !reader.readUInt32(endian: .big, &header.samplePtr) ||
		!reader.readUInt32(endian: .big, &header.length) ||
		!reader.readUInt32(endian: .big, &header.sampleRate) ||
		!reader.readUInt32(endian: .big, &header.loopStart) ||
		!reader.readUInt32(endian: .big, &header.loopEnd) ||
		!reader.readUInt8(&header.encode) ||
		!reader.readUInt8(&header.baseFrequency) {
		errmsg("Missing header data")
		throw SndAssetErrors.missingHeader
	}
	let sampleData = reader.read(Int(header.length))
	if sampleData == nil {
		errmsg("Missing samples")
		throw SndAssetErrors.missingSamples
	}
	if header.encode != stdSH {
		if header.encode == extSH {
			errmsg("Extended encoding not supported")
		} else if header.encode == cmpSH {
			errmsg("Compression not supported")
		} else {
			errmsg(String(format: "Unknown encoding 0x%02X", header.encode))
			throw SndAssetErrors.unknownEncoding(enc: header.encode)
		}
		throw MADErr.fileNotSupportedByThisPlug
	}
	
	// Generate an AudioStreamBasicDescription for conversion
	var stream = AudioStreamBasicDescription(sampleRate: fixedToFloat(header.sampleRate), formatFlags: .signedInteger, bitsPerChannel: 8, channelsPerFrame: 1)
	
	// Create a temporary file for storage
	let url = URL(fileURLWithPath: (NSTemporaryDirectory() as NSString).appendingPathComponent("\(arc4random())-\(Date.timeIntervalSinceReferenceDate).aif"))
	var audioFile1: ExtAudioFileRef? = nil
	let createStatus = ExtAudioFileCreate(url: url, fileType: .AIFF, streamDescription: &stream, flags: .eraseFile, audioFile: &audioFile1)
	if createStatus != noErr {
		errmsg("ExtAudioFileCreateWithURL failed with status \(createStatus)")
		throw MADErr.writingErr.toNSError(customUserDictionary: [NSUnderlyingErrorKey:NSError(domain: NSOSStatusErrorDomain, code: Int(createStatus))], convertToCocoa: false)!
	}
	guard let audioFile = audioFile1 else {
		throw MADErr.writingErr
	}
	defer {
		// Finish up
		let disposeStatus = ExtAudioFileDispose(audioFile)
		errmsg("ExtAudioFileDispose failed with status \(disposeStatus)")
	}
	
	// Configure the AudioBufferList
	let srcData = ((sampleData! as NSData).bytes).assumingMemoryBound(to: UInt8.self)
	var audioBuffer = AudioBuffer()
	audioBuffer.mNumberChannels = 1
	audioBuffer.mDataByteSize = header.length
	audioBuffer.mData = UnsafeMutableRawPointer(mutating: srcData)
	let audioBufferData = audioBuffer.mData?.assumingMemoryBound(to: UInt8.self)
	for i in 0 ..< Int(header.length) {
		audioBufferData?[i] ^= 0x80
	}
	var bufferList = AudioBufferList(mNumberBuffers: 1, mBuffers: audioBuffer)
	
	// Write the data to the file
	let writeStatus = ExtAudioFileWrite(audioFile, header.length, &bufferList)
	if writeStatus != noErr {
		errmsg("ExtAudioFileWrite failed with status \(writeStatus)")
		throw MADErr.writingErr.toNSError(customUserDictionary: [NSUnderlyingErrorKey:NSError(domain: NSOSStatusErrorDomain, code: Int(writeStatus))], convertToCocoa: false)!
	}
	
	return url
}
