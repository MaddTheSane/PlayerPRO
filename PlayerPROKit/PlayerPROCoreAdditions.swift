//
//  PlayerPROCoreAdditions.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import CoreFoundation
import Foundation
import PlayerPROCore.PlugIns
import SwiftAdditions

public let MadID = toOSType(string: "MADK")

// MARK: PlayerPRO MAD data types

extension MADOutputChannel: CustomStringConvertible {
	public var stringValue: String {
		switch self {
		case .MonoOutPut:
			return "mono"
			
		case .StereoOutPut:
			return "stereo"
			
		case .DeluxeStereoOutPut:
			return "deluxe stereo"
			
		case .PolyPhonic:
			return "polyphonic"
			
		@unknown default:
			return "Unknown channel layout '\(rawValue)'"
		}
	}
	
	public var description: String {
		return self.stringValue
	}
}

extension MADSoundOutput: CustomStringConvertible {
	public var stringValue: String {
		switch self {
		case .CoreAudioDriver:
			return "CoreAudio"
			
		case .DirectSound95NT:
			return "DirectSound"
			
		case .Wave95NT:
			return "Windows WaveOut"
			
		case .PortAudioDriver:
			return "PortAudio"
			
		case .PulseAudioDriver:
			return "PulseAudio"
			
		case .ESDDriver:
			return "ESound Daemon"
			
		case .BeOSSoundDriver:
			return "BeOS/Haiku"
			
		case .MIDISoundDriver:
			return "MIDI"
			
		case .ASIOSoundManager:
			return "ASIO"
			
		case .SoundManagerDriver:
			return "Carbon Sound Manager"
			
		case .NoHardwareDriver:
			return "none"
			
		@unknown default:
			return "Unknown sound output '\(rawValue)'"
		}
	}
	
	public var description: String {
		return self.stringValue
	}
	
	public var isAvailable: Bool {
		return MADSoundDriverIsAvalable(self)
	}
}

extension MADDriverSettings: CustomDebugStringConvertible, Equatable {	
	public static func new() -> MADDriverSettings {
		var mdriverSettings = MADDriverSettings()
		mdriverSettings.resetToBestDriver()
		return mdriverSettings
	}

	public mutating func resetToBestDriver() {
		MADGetBestDriver(&self)
	}
	
	public var debugDescription: String {
		let onVal = "on"
		let offVal = "off"
		return "Driver Mode: \(driverMode.description), output mode: \(outPutMode.description.capitalized); Channel count: \(numChn), output Rate: \(outPutRate), surround: \(surround == true ? onVal : offVal); micro-delay size: \(MicroDelaySize), reverb, is \(Reverb == true ? onVal: offVal), size: \(ReverbSize), strength: \(ReverbStrength); oversampling \(oversampling); repeat music: \(repeatMusic == true ? onVal : offVal); "
	}
	
	public static func ==(lhs: MADDriverSettings, rhs: MADDriverSettings) -> Bool {
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
}

public let maximumPanning: MADByte = 64
public let minimumVolume: MADByte = 0
public let noFineTune: UInt16 = 8363
public let maximumVolume: MADByte = 64
public let maximumChannelVolume: MADByte = 128
public let maximumArpeggio: Int32 = 3
public let equalizerPacketElements = 512

/// PlayerPROCore's internal debugger function, wrapped to include the line and file in Swift.
/// - parameter text: Developer text that is used to help debug the issue. **Cannot be nil**, but may be an empty string.
/// - parameter file: The file that has the problem.
/// - parameter line: the line of the source file that's having a problem.
///
/// **Normally** it is never called, only when a FATAL error has occured. <br>
/// This function is automatically invoked using the macros `#line` and
/// `#file` for the `line` and `file` paramaters, respectively.
public func MADDebug(string text: String, line: Int16 = #line, file: StaticString = #file) {
	MADDebugStr(line, (String(describing: file) as NSString).fileSystemRepresentation, text)
}

private var BlankNameChar32: (Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8,
	Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8,
	Int8, Int8, Int8, Int8, Int8, Int8) = {
	return (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
}()

extension sData32 {
	private init() {
		self.init(size: 0, loopBeg: 0, loopSize: 0, vol: maximumVolume, c2spd: noFineTune, loopType: .classic, amp: 8, realNote: 0, name: BlankNameChar32, stereo: false, data: 0)
	}
	
	public static func new() -> sData32 {
		return sData32()
	}
	
	public var toSData : sData {
		return sData(self)
	}
	
	public init(_ fromSData32: sData) {
		self.init(size: fromSData32.size, loopBeg: fromSData32.loopBeg, loopSize: fromSData32.loopSize, vol: fromSData32.vol, c2spd: fromSData32.c2spd, loopType: fromSData32.loopType, amp: fromSData32.amp, realNote: fromSData32.realNote, name: fromSData32.name, stereo: fromSData32.stereo, data: 0)
	}
}

extension sData {
	public static func new() -> sData {
		return sData()
	}
	
	public var toSData32 : sData32 {
		return sData32(self)
	}
	
	public init(_ fromSData32: sData32) {
		self.init(size: fromSData32.size, loopBeg: fromSData32.loopBeg, loopSize: fromSData32.loopSize, vol: fromSData32.vol, c2spd: fromSData32.c2spd, loopType: fromSData32.loopType, amp: fromSData32.amp, realNote: fromSData32.realNote, name: fromSData32.name, stereo: fromSData32.stereo, data: nil)
	}
	
	private init() {
		self.init(size: 0, loopBeg: 0, loopSize: 0, vol: maximumVolume, c2spd: noFineTune, loopType: .classic, amp: 8, realNote: 0, name: BlankNameChar32, stereo: false, data: nil)
	}
}

extension sData: Hashable {
	public static func ==(_ lhs: sData, _ rhs: sData) -> Bool {
		guard lhs.amp == rhs.amp else {
			return false
		}
		
		guard lhs.size == rhs.size else {
			return false
		}
		
		guard lhs.loopBeg == rhs.loopBeg else {
			return false
		}
		guard lhs.loopSize == rhs.loopSize else {
			return false
		}
		guard lhs.loopType == rhs.loopType else {
			return false
		}
		guard lhs.vol == rhs.vol else {
			return false
		}
		guard lhs.c2spd == rhs.c2spd else {
			return false
		}
		guard lhs.realNote == rhs.realNote else {
			return false
		}
		guard lhs.stereo == rhs.stereo else {
			return false
		}
		let lName = Array(UnsafeBufferPointer(start: lhs.data, count: Int(lhs.size)))
		let rName = Array(UnsafeBufferPointer(start: rhs.data, count: Int(rhs.size)))
		guard rName == lName else {
			return false
		}
		
		return true
	}
	
	public func hash(into hasher: inout Hasher) {
		hasher.combine(size)
		hasher.combine(loopBeg)
		hasher.combine(loopSize)
		hasher.combine(vol)
		hasher.combine(c2spd)
		hasher.combine(loopType)
		hasher.combine(amp)
		hasher.combine(realNote)
		hasher.combine(stereo)
		let dat = UnsafeRawPointer(data)
		let datLen = UnsafeRawBufferPointer(start: dat, count: Int(size))
		hasher.combine(bytes: datLen)
	}
}

extension EnvRec: Hashable {
	public func hash(into hasher: inout Hasher) {
		hasher.combine(pos)
		hasher.combine(val)
	}
	
	public static func ==(lhs: EnvRec, rhs: EnvRec) -> Bool {
		if lhs.pos != rhs.pos {
			return false
		}
		if lhs.val != rhs.val {
			return false
		}
		
		return true
	}
}

extension FXBus: Hashable {
	public func hash(into hasher: inout Hasher) {
		hasher.combine(copyId)
		hasher.combine(Active)
		hasher.combine(ByPass)
	}
	
	public static func ==(lhs: FXBus, rhs: FXBus) -> Bool {
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
}

extension FXSets: Equatable {
	public static func ==(lhs: FXSets, rhs: FXSets) -> Bool {
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
		let lhsName = String(pascalString: lhs.name) ?? ""
		let rhsName = String(pascalString: rhs.name) ?? ""
		if lhsName != rhsName {
			return false
		}
		
		let lhsValuesArray: [Float] = try! arrayFromObject(reflecting: lhs.values)
		let rhsValuesArray: [Float] = try! arrayFromObject(reflecting: rhs.values)
		// Ignore values that aren't accessed
		for i in 0..<Int(lhs.noArg) {
			if lhsValuesArray[i] != rhsValuesArray[i] {
				return false
			}
		}
		
		return true
	}
}

extension Cmd: Hashable {
	private init() {
		self.init(ins: 0, note: 0xFF, cmd: .arpeggio, arg: 0, vol: 0xFF, unused: 0)
	}
	
	/// Returns a `Cmd` with all bytes to zero, except `note`
	/// and `vol`, which are `0xFF`.
	public static func blankCmd() -> Cmd {
		return Cmd()
	}
	
	/// Returns a `Cmd` with all bytes to zero, except `note`
	/// and `vol`, which are `0xFF`.
	public static func new() -> Cmd {
		return Cmd()
	}
	
	/// Reset the command
	public mutating func kill() {
		MADKillCmd(&self)
	}
	
	public static func ==(lhs: Cmd, rhs: Cmd) -> Bool {
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
	
	public func hash(into hasher: inout Hasher) {
		hasher.combine(ins)
		hasher.combine(note)
		hasher.combine(cmd)
		hasher.combine(arg)
		hasher.combine(vol)
	}
}

public func getCommand(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>) -> Cmd {
	return getCommand(position: position, channel: channel, aPat: aPat).pointee
}

private func getCommand(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>) -> UnsafeMutablePointer<Cmd> {
	return GetMADCommand(position, channel, aPat)
}

public func replaceCmd(position: Int16, channel: Int16, command: Cmd, aPat: UnsafeMutablePointer<PatData>) {
	let aCmd: UnsafeMutablePointer<Cmd> = getCommand(position: position, channel: channel, aPat: aPat)
	aCmd.pointee = command
}

public func modifyCmdAtRow(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>, commandBlock: (inout Cmd)-> ()) {
	var aCmd: Cmd = getCommand(position: position, channel: channel, aPat: aPat)
	commandBlock(&aCmd)
	replaceCmd(position: position, channel: channel, command: aCmd, aPat: aPat)
}

public func getCommand(row: Int16, track: Int16, aPcmd: UnsafeMutablePointer<Pcmd>) -> Cmd {
	return MADGetCmd(row, track, aPcmd).pointee
}

public func replaceCommand(row: Int16, track: Int16, command: Cmd, aPcmd: UnsafeMutablePointer<Pcmd>) {
	let aCmd = MADGetCmd(row, track, aPcmd)!
	aCmd.pointee = command
}

public func modifyCommand(row: Int16, track: Int16, aPcmd: UnsafeMutablePointer<Pcmd>, commandBlock: (inout Cmd)-> ()) {
	var aCmd = getCommand(row: row, track: track, aPcmd: aPcmd)
	commandBlock(&aCmd)
	replaceCommand(row: row, track: track, command: aCmd, aPcmd: aPcmd)
}

extension IntPcmd: CommandIterator, Equatable {
	public var commandLength: Int16 {
		return length
	}
	
	private func getCommandIndex(atRow arow: Int16, track atrack: Int16) -> Int {
		var row = arow
		var track = atrack
		if row < 0 {
			row = 0
		} else if row >= length {
			row = length - 1
		}
		
		if track < 0 {
			track = 0
		} else if track >= tracks {
			track = tracks - 1
		}
		
		return Int(length) * Int(track) + Int(row)
	}
	
	public func getCommand(atRow row: Int16, track: Int16) -> Cmd {
		let ourAddr = getCommandIndex(atRow: row, track: track)
		
		return myCmd[ourAddr]
	}
	
	public mutating func modifyCommand(atRow row: Int16, track: Int16, commandBlock: (inout Cmd) -> ()) {
		let ourAddr = getCommandIndex(atRow: row, track: track)
		
		commandBlock(&myCmd[ourAddr])
	}
	
	public mutating func replaceCommand(atRow row: Int16, track: Int16, with command: Cmd) {
		modifyCommand(atRow: row, track: track, commandBlock: {( aCmd: inout Cmd) -> () in
			aCmd = command
		})
	}
	
	public static func ==(lhs: IntPcmd, rhs: IntPcmd) -> Bool {
		if lhs.tracks != rhs.tracks {
			return false
		} else if lhs.length != rhs.length {
			return false
		} else if lhs.trackStart != rhs.trackStart {
			return false
		} else if lhs.posStart != rhs.posStart {
			return false
		} else if lhs.cmdCount != rhs.cmdCount {
			return false
		} else {
			let lhsCmds = UnsafeMutableBufferPointer(start: lhs.myCmd, count: Int(lhs.cmdCount))
			let rhsCmds = UnsafeMutableBufferPointer(start: rhs.myCmd, count: Int(rhs.cmdCount))
			for (lhsCmd, rhsCmd) in zip(lhsCmds, rhsCmds) {
				if lhsCmd != rhsCmd {
					return false
				}
			}
		}
		
		return true
	}
}

extension MADChannel {
	public var arpeggio: (values: [Int32], index: Int32, enabled: Bool) {
		return (try! arrayFromObject(reflecting: arp), arpindex, arpUse)
	}
	
	public var vibrato: (offset: Int8, depth: Int32, rate: Int32, type: Int32) {
		return (viboffset, vibdepth, vibrate, vibtype)
	}
}

extension MADBool: ExpressibleByBooleanLiteral {
	public init(booleanLiteral value: Bool) {
		if value {
			self = .true
		} else {
			self = .false
		}
	}

	public init(_ value: Bool) {
		self.init(booleanLiteral: value)
	}

	public var boolValue: Bool {
		if rawValue == 0 {
			return false
		} else {
			return true
		}
	}
}

extension MADBool: CustomReflectable, Equatable {
	public var customMirror: Mirror {
		return Mirror(reflecting: boolValue)
	}
	
	static public func ==(lhs: MADBool, rhs: MADBool) -> Bool {
		return lhs.boolValue == rhs.boolValue
	}
}

extension MADBool: CustomPlaygroundDisplayConvertible {
	public var playgroundDescription: Any {
		return self.boolValue
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
