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

// MARK: Bridges to more modern Swift code.
public let MadID = StringToOSType("MADK")

extension MADFourChar: StringLiteralConvertible {
	public init(_ toInit: String) {
		self = StringToOSType(toInit)
	}
	
	public init(unicodeScalarLiteral usl: String) {
		let tmpUnscaled = String(unicodeScalarLiteral: usl)
		self = StringToOSType(tmpUnscaled)
	}
	
	public init(extendedGraphemeClusterLiteral egcl: String) {
		let tmpUnscaled = String(extendedGraphemeClusterLiteral: egcl)
		self.init(tmpUnscaled)
	}
	
	public init(stringLiteral toInit: String) {
		self = StringToOSType(toInit)
	}
	
	public init(_ toInit: (Int8, Int8, Int8, Int8, Int8)) {
		self = MADFourChar((toInit.0, toInit.1, toInit.2, toInit.3))
	}
	
	public var OSTypeStringValue: String? {
		return OSTypeToString(self)
	}
	
	public init(_ toInit: (Int8, Int8, Int8, Int8)) {
		let val0 = UInt32(toInit.0)
		let val1 = UInt32(toInit.1)
		let val2 = UInt32(toInit.2)
		let val3 = UInt32(toInit.3)
		self = MADFourChar((val0 << 24) | (val1 << 16) | (val2 << 8) | (val3))
	}
	
	public func toFourChar() -> (Int8, Int8, Int8, Int8) {
		let var1 = (self >> 24) & 0xFF
		let var2 = (self >> 16) & 0xFF
		let var3 = (self >> 8) & 0xFF
		let var4 = (self) & 0xFF
		return (Int8(var1), Int8(var2), Int8(var3), Int8(var4))
	}
	
	public func toFourChar() -> (Int8, Int8, Int8, Int8, Int8) {
		let outVar: (Int8, Int8, Int8, Int8) = toFourChar()
		return (outVar.0, outVar.1, outVar.2, outVar.3, 0)
	}
}

// MARK: PlayerPRO MAD data types

public func NewMADDriverSettings(setToDefault: Bool = true) -> MADDriverSettings {
	var ourSett = MADDriverSettings()
	
	if setToDefault {
		ourSett.resetToBestDriver()
	}
	
	return ourSett
}

extension MADDriverSettings: DebugPrintable, Equatable {
	public init() {
		self.driverMode = .CoreAudioDriver
		self.numChn = 4
		self.outPutBits = 16
		self.outPutMode = .DeluxeStereoOutPut
		self.outPutRate = 44100
		self.MicroDelaySize = 25
		self.ReverbSize = 25
		self.ReverbStrength = 25
		self.oversampling = 1
		self.TickRemover = false
		self.surround = false
		self.Reverb = false
		self.repeatMusic = false
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
		return CFBundleCopyBundleURL(file.takeUnretainedValue()) as NSURL
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
		return CFStringToString(MenuName.takeUnretainedValue())
	}
	
	public var author: String {
		return CFStringToString(AuthorString.takeUnretainedValue())
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
		self.vol = sData.maximumVolume
		self.c2spd = sData.noFineTune
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
	public static let noFineTune: UInt16 = 8363
	public static let minimumVolume: MADByte = 0
	public static let maximumVolume: MADByte = 64

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
		self.vol = sData.maximumVolume
		self.c2spd = sData.noFineTune
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

extension IntPcmd {
	public init() {
		tracks = 0
		length = 0
		trackStart = 0
		posStart = 0
		cmdCount = 0
		myCmd = nil
	}
	
	public func getCommand(arow: Int16, track atrack: Int16) -> Cmd {
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
		
		let ourAddr = Int(length) * Int(track) + Int(row)
		
		return myCmd[ourAddr]
	}
	
	public mutating func modifyCmdAtRow(arow: Int16, track atrack: Int16, commandBlock: (inout Cmd) -> ()) {
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
		
		let ourAddr = Int(length) * Int(track) + Int(row)
		
		commandBlock(&myCmd[ourAddr])
	}
	
	public mutating func replaceCmd(row: Int16, track: Int16, command: Cmd) {
		modifyCmdAtRow(row, track: track, commandBlock: {(inout aCmd: Cmd) -> () in
			aCmd = command
		})
	}
}

public func GetCommand(arow: Int16, atrack: Int16, aIntPcmd: IntPcmd) -> Cmd {
	return aIntPcmd.getCommand(arow, track: atrack)
}

public func ReplaceCmd(row: Int16, track: Int16, command: Cmd, inout aPcmd: IntPcmd) {
	aPcmd.replaceCmd(row, track: track, command: command)
}

public func ModifyCmdAtRow(arow: Int16, atrack: Int16, inout aPcmd: IntPcmd, commandBlock: (inout Cmd) -> ()) {
	aPcmd.modifyCmdAtRow(arow, track: atrack, commandBlock: commandBlock)
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
