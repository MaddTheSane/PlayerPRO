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

private func fixedToFloat(a: UInt32) -> Double {
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

internal func canOpenData(data: NSData) -> Bool {
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
		if !reader.readInt16(.Big, &format) {
			return false
		}
		if format == firstSoundFormat {
			var numModifiers: Int16 = 0
			var modifierPart = ModRef()
			if !reader.readInt16(.Big, &numModifiers) ||
				!reader.readUInt16(.Big, &modifierPart.modNumber) ||
				!reader.readInt32(.Big, &modifierPart.modInit) {
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
			if !reader.readInt16(.Big, &refCount) {
				return false
			}
		} else {
			return false
		}
		return true
	#endif
}

internal func assetForSND(data: NSData, inout error: MADErr) -> NSURL? {
	func errmsg(@autoclosure(escaping) message: () -> String) {
		print("Sys7 import: \(message())")
	}
	
	let reader = FVDataReader(data)
	
	// Read the SndListResource or Snd2ListResource
	var format = Int16()
	if !reader.readInt16(.Big, &format) {
		error = .FileNotSupportedByThisPlug
		errmsg("Missing header")
		return nil
	}
	if format == firstSoundFormat {
		var numModifiers = Int16()
		var modifierPart = ModRef()
		if !reader.readInt16(.Big, &numModifiers) ||
			!reader.readUInt16(.Big, &modifierPart.modNumber) ||
			!reader.readInt32(.Big, &modifierPart.modInit) {
				error = .FileNotSupportedByThisPlug
				errmsg("Missing header")
				return nil
		}
		if numModifiers != 1 {
			error = .FileNotSupportedByThisPlug
			errmsg("Bad header")
			return nil
		}
		if modifierPart.modNumber != 5  {
			error = .FileNotSupportedByThisPlug
			errmsg("Unknown modNumber value \(modifierPart.modNumber)")
			return nil
		}
		if modifierPart.modInit & initStereo == 1 {
			error = .FileNotSupportedByThisPlug
			errmsg("Only mono channel supported")
			return nil
		}
		if (modifierPart.modInit & initMACE3) == initMACE3 || (modifierPart.modInit & initMACE6) == initMACE6 {
			error = .FileNotSupportedByThisPlug
			errmsg("Compression not supported")
			return nil
		}
	} else if format == secondSoundFormat {
		var refCount = Int16()
		if !reader.readInt16(.Big, &refCount) {
			error = .FileNotSupportedByThisPlug
			errmsg("Missing header")
			return nil
		}
	} else {
		error = .FileNotSupportedByThisPlug
		errmsg("Unknown format \(format)")
		return nil
	}
	
	// Read SndCommands
	var header_offset = Int()
	var numCommands = Int16()
	var commandPart = SndCommand()
	if !reader.readInt16(.Big, &numCommands) {
		error = .FileNotSupportedByThisPlug
		errmsg("Missing header")
		return nil
	}
	if numCommands == 0 {
		error = .FileNotSupportedByThisPlug
		errmsg("Bad header")
		return nil
	}
	for var i = Int16(0); i < numCommands; ++i {
		if !reader.readUInt16(.Big, &commandPart.cmd) ||
			!reader.readInt16(.Big, &commandPart.param1) ||
			!reader.readInt32(.Big, &commandPart.param2) {
				error = .FileNotSupportedByThisPlug
				errmsg("Missing command")
				return nil
		}
		// "If soundCmd is contained within an 'snd ' resource, the high bit of the command must be set."
		// Apple docs says this for bufferCmd as well, so we clear the bit.
		commandPart.cmd &= ~0x8000
		switch commandPart.cmd {
		case soundCmd, bufferCmd:
			if header_offset != 0 {
				error = .FileNotSupportedByThisPlug
				errmsg("Duplicate commands")
				return nil
			}
			header_offset = Int(commandPart.param2)
		case nullCmd:
			break
		default:
			error = .FileNotSupportedByThisPlug
			errmsg("Unknown command \(commandPart.cmd)")
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
			error = .FileNotSupportedByThisPlug
			errmsg("Missing header data")
			return nil
	}
	let sampleData = reader.read(Int(header.length))
	if sampleData == nil {
		error = .FileNotSupportedByThisPlug
		errmsg("Missing samples")
		return nil
	}
	if header.encode != stdSH {
		if header.encode == extSH {
			error = .FileNotSupportedByThisPlug
			errmsg("Extended encoding not supported")
		} else if header.encode == cmpSH {
			error = .FileNotSupportedByThisPlug
			errmsg("Compression not supported")
		} else {
			error = .FileNotSupportedByThisPlug
			errmsg(String(format: "Unknown encoding 0x%02X", header.encode))
		}
		return nil
	}
	
	// Generate an AudioStreamBasicDescription for conversion
	var stream = AudioStreamBasicDescription(sampleRate: fixedToFloat(header.sampleRate), formatFlags: .SignedInteger, bitsPerChannel: 8, channelsPerFrame: 1)
	
	// Create a temporary file for storage
	let url = NSURL(fileURLWithPath: (NSTemporaryDirectory() as NSString).stringByAppendingPathComponent("\(arc4random())-\(NSDate.timeIntervalSinceReferenceDate()).aif"))
		var audioFile: ExtAudioFileRef = nil
		let createStatus = ExtAudioFileCreate(URL: url, fileType: .AIFF, streamDescription: &stream, flags: .EraseFile, audioFile: &audioFile)
		if createStatus != noErr {
			error = .WritingErr
			errmsg("ExtAudioFileCreateWithURL failed with status \(createStatus)")
			return nil
		}
		
		// Configure the AudioBufferList
		let srcData = UnsafePointer<UInt8>(sampleData!.bytes)
		var audioBuffer = AudioBuffer()
		audioBuffer.mNumberChannels = 1
		audioBuffer.mDataByteSize = header.length
		audioBuffer.mData = UnsafeMutablePointer(srcData)
		let audioBufferData = UnsafeMutablePointer<UInt8>(audioBuffer.mData)
		for var i = 0; i < Int(header.length); ++i {
			audioBufferData[i] ^= 0x80
		}
		var bufferList = AudioBufferList(mNumberBuffers: 1, mBuffers: audioBuffer)
		
		// Write the data to the file
		let writeStatus = ExtAudioFileWrite(audioFile, header.length, &bufferList)
		if writeStatus != noErr {
			error = .WritingErr
			errmsg("ExtAudioFileWrite failed with status \(writeStatus)")
			return nil
		}
		
		// Finish up
		let disposeStatus = ExtAudioFileDispose(audioFile)
		if disposeStatus != noErr {
			error = .WritingErr
			errmsg("ExtAudioFileDispose failed with status \(disposeStatus)")
			return nil
		}
		
		// Generate an AVAsset
		return url
}
