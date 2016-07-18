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

internal func assetForSND(_ data: Data, error: inout MADErr) -> URL? {
	func errmsg( _ message: @autoclosure(escaping) () -> String) {
		print("Sys7 import: \(message())")
	}
	
	let reader = FVDataReader(data)
	
	// Read the SndListResource or Snd2ListResource
	var format = Int16()
	if !reader.readInt16(endian: .big, &format) {
		error = .fileNotSupportedByThisPlug
		errmsg("Missing header")
		return nil
	}
	if format == firstSoundFormat {
		var numModifiers = Int16()
		var modifierPart = ModRef()
		if !reader.readInt16(endian: .big, &numModifiers) ||
			!reader.readUInt16(endian: .big, &modifierPart.modNumber) ||
			!reader.readInt32(endian: .big, &modifierPart.modInit) {
				error = .fileNotSupportedByThisPlug
				errmsg("Missing header")
				return nil
		}
		if numModifiers != 1 {
			error = .fileNotSupportedByThisPlug
			errmsg("Bad header")
			return nil
		}
		if modifierPart.modNumber != 5  {
			error = .fileNotSupportedByThisPlug
			errmsg("Unknown modNumber value \(modifierPart.modNumber)")
			return nil
		}
		if modifierPart.modInit & initStereo == 1 {
			error = .fileNotSupportedByThisPlug
			errmsg("Only mono channel supported")
			return nil
		}
		if (modifierPart.modInit & initMACE3) == initMACE3 || (modifierPart.modInit & initMACE6) == initMACE6 {
			error = .fileNotSupportedByThisPlug
			errmsg("Compression not supported")
			return nil
		}
	} else if format == secondSoundFormat {
		var refCount = Int16()
		if !reader.readInt16(endian: .big, &refCount) {
			error = .fileNotSupportedByThisPlug
			errmsg("Missing header")
			return nil
		}
	} else {
		error = .fileNotSupportedByThisPlug
		errmsg("Unknown format \(format)")
		return nil
	}
	
	// Read SndCommands
	var header_offset = Int()
	var numCommands = Int16()
	var commandPart = SndCommand()
	if !reader.readInt16(endian: .big, &numCommands) {
		error = .fileNotSupportedByThisPlug
		errmsg("Missing header")
		return nil
	}
	if numCommands == 0 {
		error = .fileNotSupportedByThisPlug
		errmsg("Bad header")
		return nil
	}
	for _ in Int16(0) ..< numCommands {
		if !reader.readUInt16(endian: .big, &commandPart.cmd) ||
			!reader.readInt16(endian: .big, &commandPart.param1) ||
			!reader.readInt32(endian: .big, &commandPart.param2) {
				error = .fileNotSupportedByThisPlug
				errmsg("Missing command")
				return nil
		}
		// "If soundCmd is contained within an 'snd ' resource, the high bit of the command must be set."
		// Apple docs says this for bufferCmd as well, so we clear the bit.
		commandPart.cmd &= ~0x8000
		switch commandPart.cmd {
		case soundCmd, bufferCmd:
			if header_offset != 0 {
				error = .fileNotSupportedByThisPlug
				errmsg("Duplicate commands")
				return nil
			}
			header_offset = Int(commandPart.param2)
		case nullCmd:
			break
		default:
			error = .fileNotSupportedByThisPlug
			errmsg("Unknown command \(commandPart.cmd)")
			return nil
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
			error = .fileNotSupportedByThisPlug
			errmsg("Missing header data")
			return nil
	}
	let sampleData = reader.read(Int(header.length))
	if sampleData == nil {
		error = .fileNotSupportedByThisPlug
		errmsg("Missing samples")
		return nil
	}
	if header.encode != stdSH {
		if header.encode == extSH {
			error = .fileNotSupportedByThisPlug
			errmsg("Extended encoding not supported")
		} else if header.encode == cmpSH {
			error = .fileNotSupportedByThisPlug
			errmsg("Compression not supported")
		} else {
			error = .fileNotSupportedByThisPlug
			errmsg(String(format: "Unknown encoding 0x%02X", header.encode))
		}
		return nil
	}
	
	// Generate an AudioStreamBasicDescription for conversion
	var stream = AudioStreamBasicDescription(sampleRate: fixedToFloat(header.sampleRate), formatFlags: .SignedInteger, bitsPerChannel: 8, channelsPerFrame: 1)
	
	// Create a temporary file for storage
	let url = URL(fileURLWithPath: (NSTemporaryDirectory() as NSString).appendingPathComponent("\(arc4random())-\(Date.timeIntervalSinceReferenceDate).aif"))
		var audioFile: ExtAudioFileRef? = nil
		let createStatus = ExtAudioFileCreate(URL: url, fileType: .AIFF, streamDescription: &stream, flags: .eraseFile, audioFile: &audioFile)
		if createStatus != noErr {
			error = .writingErr
			errmsg("ExtAudioFileCreateWithURL failed with status \(createStatus)")
			return nil
		}
		
		// Configure the AudioBufferList
		let srcData = UnsafePointer<UInt8>((sampleData! as NSData).bytes)
		var audioBuffer = AudioBuffer()
		audioBuffer.mNumberChannels = 1
		audioBuffer.mDataByteSize = header.length
		audioBuffer.mData = UnsafeMutablePointer(srcData)
		let audioBufferData = UnsafeMutablePointer<UInt8>(audioBuffer.mData)
		for i in 0 ..< Int(header.length) {
			audioBufferData?[i] ^= 0x80
		}
		var bufferList = AudioBufferList(mNumberBuffers: 1, mBuffers: audioBuffer)
		
		// Write the data to the file
		let writeStatus = ExtAudioFileWrite(audioFile!, header.length, &bufferList)
		if writeStatus != noErr {
			error = .writingErr
			errmsg("ExtAudioFileWrite failed with status \(writeStatus)")
			return nil
		}
		
		// Finish up
		let disposeStatus = ExtAudioFileDispose(audioFile!)
		if disposeStatus != noErr {
			error = .writingErr
			errmsg("ExtAudioFileDispose failed with status \(disposeStatus)")
			return nil
		}
		
		// Generate an AVAsset
		return url
}
