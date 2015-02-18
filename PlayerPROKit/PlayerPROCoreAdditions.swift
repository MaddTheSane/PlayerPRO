//
//  PlayerPROCoreAdditions.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import CoreFoundation
import Foundation
import PlayerPROCore
import SwiftAdditions

public func ==(lhs: Cmd, rhs: Cmd) -> Bool {
	// Don't worry about the "unused" variable for now
	if lhs.ins != rhs.ins {
		return false
	}
	if lhs.note != rhs.note {
		return false
	}
	if lhs.cmd != rhs.cmd {
		return false
	}
	if lhs.arg != rhs.arg {
		return false
	}
	if lhs.vol != rhs.vol {
		return false
	}
	
	return true
}

public func ==(lhs: EnvRec, rhs: EnvRec) -> Bool {
	if lhs.pos != rhs.pos {
		return false
	}
	if lhs.val != rhs.val {
		return false
	}
	
	return true
}

public func ==(lhs: FXBus, rhs: FXBus) -> Bool {
	if lhs.Active != rhs.Active {
		return false
	}
	if lhs.ByPass != rhs.ByPass {
		return false
	}
	if lhs.copyId != rhs.copyId {
		return false
	}
	
	return true
}

public func ==(lhs: MADDriverSettings, rhs: MADDriverSettings) -> Bool {
	if lhs.driverMode != rhs.driverMode {
		return false
	}
	if lhs.numChn != rhs.numChn {
		return false
	}
	if lhs.outPutBits != rhs.outPutBits {
		return false
	}
	if lhs.outPutMode != rhs.outPutMode {
		return false
	}
	if lhs.outPutRate != rhs.outPutRate {
		return false
	}
	if lhs.MicroDelaySize != rhs.MicroDelaySize {
		return false
	}
	if lhs.ReverbSize != rhs.ReverbSize {
		return false
	}
	if lhs.ReverbStrength != rhs.ReverbStrength {
		return false
	}
	if lhs.oversampling != rhs.oversampling {
		return false
	}
	if lhs.TickRemover != rhs.TickRemover {
		return false
	}
	if lhs.surround != rhs.surround {
		return false
	}
	if lhs.Reverb != rhs.Reverb {
		return false
	}
	if lhs.repeatMusic != rhs.repeatMusic {
		return false
	}
	
	return true
}

public func ==(lhs: FXSets, rhs: FXSets) -> Bool {
	//return lhs.theSet == rhs
	if lhs.track != rhs.track {
		return false
	}
	if lhs.id != rhs.id {
		return false
	}
	if lhs.FXID != rhs.FXID {
		return false
	}
	if lhs.noArg != rhs.noArg {
		return false
	}
	let lhsNameMirror = reflect(lhs.name)
	let rhsNameMirror = reflect(rhs.name)
	let lhsNameArray: [UInt8] = GetArrayFromMirror(lhsNameMirror)
	let rhsNameArray: [UInt8] = GetArrayFromMirror(rhsNameMirror)
	let lhsName = (CFStringCreateWithPascalString(kCFAllocatorDefault, lhsNameArray, CFStringBuiltInEncodings.MacRoman.rawValue) as! String?) ?? ""
	let rhsName = (CFStringCreateWithPascalString(kCFAllocatorDefault, rhsNameArray, CFStringBuiltInEncodings.MacRoman.rawValue) as! String?) ?? ""
	if lhsName != rhsName {
		return false
	}
	
	let lhsValuesArray: [Float] = GetArrayFromMirror(reflect(lhs.values))
	let rhsValuesArray: [Float] = GetArrayFromMirror(reflect(rhs.values))
	// Ignore values that aren't accessed
	for i in 0..<Int(lhs.noArg) {
		if lhsValuesArray[i] != rhsValuesArray[i] {
			return false
		}
	}

	return true
}

public let MadID = StringToOSType("MADK")

// MARK: PlayerPRO MAD data types

extension MADDriverSettings: DebugPrintable, Equatable {
	public init() {
		numChn			= 4
		outPutBits		= 16
		outPutMode		= .DeluxeStereoOutPut
		outPutRate		= 44100
		MicroDelaySize	= 25
		ReverbSize		= 100
		ReverbStrength	= 20
		oversampling	= 1
		TickRemover		= false
		surround		= false
		Reverb			= false
		repeatMusic		= true
		//reserved = 0
		//Just going to use CoreAudio
		driverMode		= .CoreAudioDriver;
	}

	public mutating func resetToBestDriver() {
		MADGetBestDriver(&self)
	}
	
	public var debugDescription: String { get {
		let onVal = "on"
		let offVal = "off"
		return "Driver Mode: \(driverMode), output mode: \(outPutMode); Channel count: \(numChn), output Rate: \(outPutRate), surround: \(surround == true ? onVal : offVal); micro-delay size: \(MicroDelaySize), reverb, is \(Reverb == true ? onVal: offVal), size: \(ReverbSize), strength: \(ReverbStrength); oversampling \(oversampling); repeat music: \(repeatMusic == true ? onVal : offVal); "
		}}
}

extension MADInfoRec: DebugPrintable {
	public var internalName: String! {
		let mirror = reflect(internalFileName)
		let toParse: [CChar] = GetArrayFromMirror(mirror, appendLastObject: 0)
		return String(CString: toParse, encoding: NSMacOSRomanStringEncoding)
	}
	
	public var format: String! {
		let mirror = reflect(formatDescription)
		let toParse: [CChar] = GetArrayFromMirror(mirror, appendLastObject: 0)
		return String(CString: toParse, encoding: NSMacOSRomanStringEncoding)
	}
	
	public var debugDescription: String {
		return "\(internalName), format \(format)"
	}
}

public let maximumPanning: MADByte = 64
public let minimumVolume: MADByte = 0
public let noFineTune: UInt16 = 8363
public let maximumVolume: MADByte = 64
public let maximumChannelVolume: MADByte = 128
public let maximumArpeggio: Int32 = 3
public let equalizerPacketElements = 512

public func MADDebugString(text: String, line: UWord = __LINE__, file: StaticString = __FILE__) {
	MADDebugStr(Int16(line), file.stringValue.fileSystemRepresentation(), text.cStringUsingEncoding(NSUTF8StringEncoding)!)
}

private let BlankNameChar32: (Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8) = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

extension sData32 {
	public init() {
		self.size = 0
		self.loopBeg = 0
		self.loopSize = 0
		self.vol = maximumVolume
		self.c2spd = noFineTune
		self.loopType = .Classic
		self.amp = 8
		self.relNote = 0
		self.name = BlankNameChar32
		self.stereo = false
		self.data = 0
	}
	
	public var toSData : sData {
		var toRet = sData()
		toRet.size = self.size
		toRet.loopBeg = self.loopBeg
		toRet.loopSize = self.loopSize
		toRet.vol = self.vol
		toRet.c2spd = self.c2spd
		toRet.loopType = self.loopType
		toRet.amp = self.amp
		toRet.relNote = self.relNote
		toRet.name = self.name
		toRet.stereo = self.stereo
		
		return toRet
	}
}

extension sData {
	public var toSData32 : sData32 {
		var toRet = sData32()
		toRet.size = self.size
		toRet.loopBeg = self.loopBeg
		toRet.loopSize = self.loopSize
		toRet.vol = self.vol
		toRet.c2spd = self.c2spd
		toRet.loopType = self.loopType
		toRet.amp = self.amp
		toRet.relNote = self.relNote
		toRet.name = self.name
		toRet.stereo = self.stereo
		
		return toRet
	}
	
	public init() {
		self.size = 0
		self.loopBeg = 0
		self.loopSize = 0
		self.vol = maximumVolume
		self.c2spd = noFineTune
		self.loopType = .Classic
		self.amp = 8
		self.relNote = 0
		self.name = BlankNameChar32
		self.stereo = false
		self.data = nil
	}
}

extension EnvRec: Equatable {
	public init() {
		pos = 0
		val = 0
	}
}

extension FXBus: Equatable {
	public init() {
		Active = false
		ByPass = false
		copyId = 0
	}
}

extension FXSets: Equatable {
}

extension Cmd: Equatable {
	public init() {
		ins = 0
		note = 0xFF
		cmd = .Arpeggio
		arg = 0
		vol = 0xFF
		unused = 0
	}
	
	/// Reset the command
	public mutating func kill() {
		MADKillCmd(&self)
	}
}

public func getCommand(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>) -> Cmd {
	return getCommand(position, channel, aPat).memory
}

private func getCommand(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>) -> UnsafeMutablePointer<Cmd> {
	return GetMADCommand(position, channel, aPat)
}

public func ReplaceCmd(position: Int16, channel: Int16, command: Cmd, aPat: UnsafeMutablePointer<PatData>) {
	var aCmd: UnsafeMutablePointer<Cmd> = getCommand(position, channel, aPat)
	aCmd.memory = command
}

public func ModifyCmdAtRow(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>, commandBlock: (inout Cmd)-> ()) {
	var aCmd: Cmd = getCommand(position, channel, aPat)
	commandBlock(&aCmd)
	ReplaceCmd(position, channel, aCmd, aPat)
}

public func getCommand(#row: Int16, #track: Int16, #aPcmd: UnsafeMutablePointer<Pcmd>) -> Cmd {
	return MADGetCmd(row, track, aPcmd).memory
}

public func replaceCommand(#row: Int16, #track: Int16, #command: Cmd, #aPcmd: UnsafeMutablePointer<Pcmd>) {
	var aCmd = MADGetCmd(row, track, aPcmd)
	aCmd.memory = command
}

public func modifyCommand(#row: Int16, #track: Int16, #aPcmd: UnsafeMutablePointer<Pcmd>, #commandBlock: (inout Cmd)-> ()) {
	var aCmd = getCommand(row: row, track: track, aPcmd: aPcmd)
	commandBlock(&aCmd)
	replaceCommand(row: row, track: track, command: aCmd, aPcmd: aPcmd)
}

extension IntPcmd: CommandIterator {
	public init() {
		tracks = 0
		length = 0
		trackStart = 0
		posStart = 0
		cmdCount = 0
		myCmd = nil
	}
	
	public var commandLength: Int16 {
		return length
	}
	
	public var commandTracks: Int16 {
		return tracks
	}
	
	private func getCommandIndex(row arow: Int16, track atrack: Int16) -> Int {
		var row = arow
		var track = atrack
		if (row < 0) {
			row = 0;
		} else if (row >= length) {
			row = length - 1;
		}
		
		if (track < 0) {
			track = 0;
		} else if (track >= tracks) {
			track = tracks - 1;
		}
		
		return Int(length) * Int(track) + Int(row)
	}
	
	public func getCommand(#row: Int16, track: Int16) -> Cmd {
		let ourAddr = getCommandIndex(row: row, track: track)
		
		return myCmd[ourAddr]
	}
	
	public mutating func modifyCommand(#row: Int16, track: Int16, commandBlock: (inout Cmd) -> ()) {
		let ourAddr = getCommandIndex(row: row, track: track)
		
		commandBlock(&myCmd[ourAddr])
	}
	
	public mutating func replaceCommand(#row: Int16, track: Int16, command: Cmd) {
		modifyCommand(row: row, track: track, commandBlock: {(inout aCmd: Cmd) -> () in
			aCmd = command
		})
	}
}

extension MADChannel {
	public var arpeggio: (values: [Int32], index: Int32, enabled: Bool) {
		return (GetArrayFromMirror(reflect(arp)), arpindex, arpUse)
	}
	
	public var vibrato: (offset: Int8, depth: Int32, rate: Int32, type: Int32) {
		return (viboffset, vibdepth, vibrate, vibtype)
	}
}

// MARK: MADFourChar
// TODO: find out how Apple does this with CGFloat...
/*
public struct MADFourChar {
	let value: OSType
	typealias internalValue = OSType
	init(_ type: OSType) {
		value = type
	}
}
*/
