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
	let lhsName = (CFStringCreateWithPascalString(kCFAllocatorDefault, lhsNameArray, CFStringBuiltInEncodings.MacRoman.rawValue) as String?) ?? ""
	let rhsName = (CFStringCreateWithPascalString(kCFAllocatorDefault, rhsNameArray, CFStringBuiltInEncodings.MacRoman.rawValue) as String?) ?? ""
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
		return NSString(CString: toParse, encoding: NSMacOSRomanStringEncoding)
	}
	
	public var format: String! {
		let mirror = reflect(formatDescription)
		let toParse: [CChar] = GetArrayFromMirror(mirror, appendLastObject: 0)
		return NSString(CString: toParse, encoding: NSMacOSRomanStringEncoding)
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

extension PlugInfo {
	public var importer: Bool {
			switch (self.mode) {
			case MADPlugModes.Import.rawValue, MADPlugModes.ImportExport.rawValue:
				return true

			default:
				return false
			}
	}

	public var exporter: Bool {
			switch (self.mode) {
			case MADPlugModes.Export.rawValue, MADPlugModes.ImportExport.rawValue:
				return true

			default:
				return false
			}
	}
	
	public var plugInURL: NSURL {
		return CFBundleCopyBundleURL(file.takeUnretainedValue())
	}
	
	public var plugInBundle: NSBundle? {
		return NSBundle(URL: CFBundleCopyBundleURL(file.takeUnretainedValue()))
	}
	
	public var types: [String] {
		return UTItypes.takeUnretainedValue() as NSArray as [String]
	}
	
	public var fourCharType: MADFourChar {
		return MADFourChar(type)
	}
	
	public var name: String {
		return MenuName.takeUnretainedValue()
	}
	
	public var author: String {
		return AuthorString.takeUnretainedValue()
	}
}

public struct MADLibraryGenerator: GeneratorType {
	private let maxPlugs: Int
	private var currentPlug = 0
	private let currentLib: MADLibrary
    mutating public func next() -> PlugInfo? {
		if currentPlug >= maxPlugs {
			return nil
		} else {
			return currentLib.ThePlug[currentPlug++]
		}
    }
	
	internal init(library: MADLibrary) {
		currentLib = library
		maxPlugs = library.count
	}
}

extension MADLibrary: SequenceType {
	public var count: Int {
		return Int(TotalPlug)
	}
	
    public func generate() -> MADLibraryGenerator {
        return MADLibraryGenerator(library: self)
    }
}

public func MADDebugString(line: UWord = __LINE__, file: String = __FILE__, text: String) {
	MADDebugStr(Int16(line), file.fileSystemRepresentation(), text.cStringUsingEncoding(NSUTF8StringEncoding)!)
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
	public var setsName: String {
		if let aStr = String(pascalString: name) {
			return aStr
		} else {
			return ""
		}
	}
	
	public var floatValues: [Float] {
		let aMirror = reflect(values)
		return GetArrayFromMirror(aMirror)
	}
	
	public init() {
		track = 0
		id = 0
		FXID = 0
		noArg = 0
		values = (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
		name = (2, 72, 105, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
	}
}

extension InstrData {
}

extension Cmd: Equatable {
	public init() {
		ins = 0
		note = 0xFF
		cmd = 0
		arg = 0
		vol = 0xFF
		unused = 0
	}
	
	public mutating func kill() {
		MADKillCmd(&self)
	}
}

public func GetCommand(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>) -> Cmd {
	return GetCommand(position, channel, aPat).memory
}

private func GetCommand(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>) -> UnsafeMutablePointer<Cmd> {
	return GetMADCommand(position, channel, aPat)
}

public func ReplaceCmd(position: Int16, channel: Int16, command: Cmd, aPat: UnsafeMutablePointer<PatData>) {
	var aCmd: UnsafeMutablePointer<Cmd> = GetCommand(position, channel, aPat)
	aCmd.memory = command
}

public func ModifyCmdAtRow(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>, commandBlock: (inout Cmd)-> ()) {
	var aCmd: Cmd = GetCommand(position, channel, aPat)
	commandBlock(&aCmd)
	ReplaceCmd(position, channel, aCmd, aPat)
}

public func GetCommand(row: Int16, track: Int16, aPcmd: UnsafeMutablePointer<Pcmd>) -> Cmd {
	return GetCommand(row, track, aPcmd).memory
}

private func GetCommand(row: Int16, track: Int16, aPcmd: UnsafeMutablePointer<Pcmd>) -> UnsafeMutablePointer<Cmd> {
	return MADGetCmd(row, track, aPcmd)
}

public func ReplaceCmd(row: Int16, track: Int16, command: Cmd, aPcmd: UnsafeMutablePointer<Pcmd>) {
	var aCmd: UnsafeMutablePointer<Cmd> = GetCommand(row, track, aPcmd)
	aCmd.memory = command
}

public func ModifyCmdAtRow(row: Int16, track: Int16, aPcmd: UnsafeMutablePointer<Pcmd>, commandBlock: (inout Cmd)-> ()) {
	var aCmd: Cmd = GetCommand(row, track, aPcmd)
	commandBlock(&aCmd)
	ReplaceCmd(row, track, aCmd, aPcmd)
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
