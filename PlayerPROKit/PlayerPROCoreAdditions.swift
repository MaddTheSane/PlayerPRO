//
//  PlayerPROCoreAdditions.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Foundation
import PlayerPROCore

#if false
internal func CFStringToString(cfStr: CFString!) -> String? {
	if cfStr == nil {
		return nil
	} else {
		return cfStr! as NSString as String
	}
}
#endif

internal func CFStringToString(cfStr: CFString) -> String {
	return cfStr as NSString as String
}

internal func StringToCFString(string: String) -> CFString {
	return string as NSString as CFString
}

// MARK: Bridges to more modern Swift code.
#if os(OSX)

import CoreServices

public let PlugMenuNameKey = CFStringToString(kMadPlugMenuNameKey)
public let PlugAuthorNameKey = CFStringToString(kMadPlugAuthorNameKey)
public let PlugUTITypesKey = CFStringToString(kMadPlugUTITypesKey)
public let PlugTypeKey = CFStringToString(kMadPlugTypeKey)
public let PlugDoesImport = CFStringToString(kMadPlugDoesImport)
public let PlugDoesExport = CFStringToString(kMadPlugDoesExport)
public let PlugModeKey = CFStringToString(kMadPlugModeKey)

public let MadID: MADFourChar = "MADK"

extension MADFourChar: StringLiteralConvertible {
	public var stringValue: String {
		get {
			let toRet = UTCreateStringForOSType(self as OSType).takeRetainedValue()
			return CFStringToString(toRet)
		}}
	
	public init(_ toInit: String) {
		self = UTGetOSTypeFromString(StringToCFString(toInit))
	}
	
	/*
	public init(_ toInit: (Int8, Int8, Int8, Int8, Int8)) {
		//This is the only reliable way I got to get the string value from a C char array.
		var toParse: String = ""
		var mirror = reflect(toInit)
		for i in 0..<mirror.count {
			var aChar = mirror[i].1.value
			toParse += NSString(bytes: &aChar, length: 1, encoding: NSMacOSRomanStringEncoding)
		}
		self = MADFourChar(toParse)
	}
	*/
	
	public init(_ toInit: (Int8, Int8, Int8, Int8, Int8)) {
		self = MADFourChar((toInit.0, toInit.1, toInit.2, toInit.3))
	}

	public init(_ toInit: (Int8, Int8, Int8, Int8)) {
		let val0 = MADFourChar(toInit.0)
		let val1 = MADFourChar(toInit.1)
		let val2 = MADFourChar(toInit.2)
		let val3 = MADFourChar(toInit.3)
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

	public static func convertFromStringLiteral(value: String) -> MADFourChar {
		return MADFourChar(value)
	}
	
	public static func convertFromExtendedGraphemeClusterLiteral(value: String) -> MADFourChar {
		var tmpStr = String.convertFromExtendedGraphemeClusterLiteral(value)
		return self.convertFromStringLiteral(tmpStr)
	}
	
}
#endif

extension MADBool : BooleanLiteralConvertible, BooleanType {
	public init(_ v : BooleanType) {
		if v.boolValue {
			self = 1
		} else {
			self = 0
		}
	}
	
	public static func convertFromBooleanLiteral(value: Bool) -> MADBool {
		if (value == true) {
			return 1
		} else {
			return 0
		}
	}
	
	public var boolValue: Bool { get {
		if (self == 0) {
			return false
		} else {
			return true
		}
		}}
}

// MARK: PlayerPRO MAD data types

extension MADDriverSettings: DebugPrintable {
	public init(bestDriver: Bool = false) {
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
		if bestDriver {
			resetToBestDriver()
		}
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
	public var debugDescription: String { get {
		return ""
		}}
}

extension PlugInfo {
	public var importer: Bool {
		get {
			switch (self.mode) {
			case MADPlugModes.Import.toRaw(), MADPlugModes.ImportExport.toRaw():
				return true

			default:
				return false
			}
		}
	}

	public var exporter: Bool {
		get {
			switch (self.mode) {
			case MADPlugModes.Export.toRaw(), MADPlugModes.ImportExport.toRaw():
				return true

			default:
				return false
			}
		}
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
		maxPlugs = Int(library.TotalPlug)
	}
}

extension MADLibrary: SequenceType {
    public func generate() -> MADLibraryGenerator {
        return MADLibraryGenerator(library: self)
    }
}

private let BlankNameChar32: (Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8, Int8) = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

extension sData32 {
	public init() {
		self.size = 0
		self.loopBeg = 0
		self.loopSize = 0
		self.vol = sData.MaxVolume
		self.c2spd = sData.NoFineTune
		self.loopType = .eClassicLoop
		self.amp = 8
		self.relNote = 0
		self.name = BlankNameChar32
		self.stereo = 0
		self.data = 0
	}
	
	public var bigEndian: sData32 { get {
		var toRet = self
		toRet.size = self.size.bigEndian
		toRet.loopBeg = self.loopBeg.bigEndian
		toRet.loopSize = self.loopBeg.bigEndian
		toRet.c2spd = self.c2spd.bigEndian
		//Not byte-swapping data, as it isn't used
		
		return toRet
	}}

	public var toSData : sData { get {
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
		}}
}

extension sData {
	public static let MaxVolume: MADByte = 64
	public static let NoFineTune: UInt16 = 8363

	public var bigEndian: sData { get {
		var toRet = self
		toRet.size = self.size.bigEndian
		toRet.loopBeg = self.loopBeg.bigEndian
		toRet.loopSize = self.loopBeg.bigEndian
		toRet.c2spd = self.c2spd.bigEndian
		
		return toRet
	}}

	public var toSData32 : sData32 { get {
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
		}}
	
	public init() {
		self.size = 0
		self.loopBeg = 0
		self.loopSize = 0
		self.vol = sData.MaxVolume
		self.c2spd = sData.NoFineTune
		self.loopType = .eClassicLoop
		self.amp = 8
		self.relNote = 0
		self.name = BlankNameChar32
		self.stereo = 0
		self.data = nil
	}
}

extension EnvRec {
	public init() {
		pos = 0
		val = 0
	}

	public var bigEndian: EnvRec {get {
		return EnvRec(pos: self.pos.bigEndian, val: self.val.bigEndian)
	}}
}

extension FXBus {
	public init() {
		Active = false
		ByPass = false
		copyId = 0
	}
	
	public var bigEndian: FXBus {get {
		return FXBus(ByPass: self.ByPass, copyId: self.copyId.bigEndian, Active: self.Active)
	}}
}

extension InstrData {
	public var bigEndian: InstrData { get {
		var toRet = self;
		toRet.numSamples = self.numSamples.bigEndian
		toRet.firstSample = self.firstSample.bigEndian
		toRet.volFade = self.volFade.bigEndian
		
		toRet.MIDI = self.MIDI.bigEndian
		toRet.MIDIType = self.MIDIType.bigEndian
		
		toRet.volEnv = (self.volEnv.0.bigEndian, self.volEnv.1.bigEndian,
			self.volEnv.2.bigEndian, self.volEnv.3.bigEndian, self.volEnv.4.bigEndian,
			self.volEnv.5.bigEndian, self.volEnv.6.bigEndian, self.volEnv.7.bigEndian,
			self.volEnv.8.bigEndian, self.volEnv.9.bigEndian, self.volEnv.10.bigEndian,
			self.volEnv.11.bigEndian)
		
		toRet.pannEnv = (self.pannEnv.0.bigEndian, self.pannEnv.1.bigEndian,
			self.pannEnv.2.bigEndian, self.pannEnv.3.bigEndian, self.pannEnv.4.bigEndian,
			self.pannEnv.5.bigEndian, self.pannEnv.6.bigEndian, self.pannEnv.7.bigEndian,
			self.pannEnv.8.bigEndian, self.pannEnv.9.bigEndian, self.pannEnv.10.bigEndian,
			self.pannEnv.11.bigEndian)

		toRet.pitchEnv = (self.pitchEnv.0.bigEndian, self.pitchEnv.1.bigEndian,
			self.pitchEnv.2.bigEndian, self.pitchEnv.3.bigEndian, self.pitchEnv.4.bigEndian,
			self.pitchEnv.5.bigEndian, self.pitchEnv.6.bigEndian, self.pitchEnv.7.bigEndian,
			self.pitchEnv.8.bigEndian, self.pitchEnv.9.bigEndian, self.pitchEnv.10.bigEndian,
			self.pitchEnv.11.bigEndian)
		
		return toRet
	}}
}

extension Cmd {
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
	return GetMADCommand(position, channel, aPat).memory
}

public func GetCommand(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>) -> UnsafeMutablePointer<Cmd> {
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

extension PatHeader {
	public var bigEndian: PatHeader {get {
		var toRet = self
		toRet.size = self.size.bigEndian
		toRet.compMode = self.compMode.bigEndian
		toRet.patBytes = self.patBytes.bigEndian
		toRet.unused2 = self.unused2.bigEndian
		
		return toRet
	}}
}

// MARK: Plug-in functions

public func GetCommand(row: Int16, track: Int16, aPcmd: UnsafeMutablePointer<Pcmd>) -> Cmd {
	return MADGetCmd(row, track, aPcmd).memory
}

public func GetCommand(row: Int16, track: Int16, aPcmd: UnsafeMutablePointer<Pcmd>) -> UnsafeMutablePointer<Cmd> {
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

public let kPlayerPROFiltersPlugTypeID = CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x79, 0xEA, 0x82, 0xAD, 0x5A, 0x53, 0x46, 0xAF, 0x82, 0xA9, 0x4A, 0x06, 0x85, 0xB4, 0x58, 0x8C)!
public let kPlayerPROFiltersPlugInterfaceID = CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xDA, 0x70, 0x82, 0xA2, 0xFE, 0xF1, 0x44, 0x75, 0xB1, 0xA4, 0x35, 0xC8, 0x1E, 0xD5, 0xDB, 0x8F)!
public let kPlayerPROInstrumentPlugTypeID = CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xFD, 0x71, 0x54, 0xD6, 0x20, 0xBF, 0x40, 0x07, 0x88, 0x1B, 0x8E, 0x44, 0x97, 0x0C, 0x3B, 0x0A)!
public let kPlayerPROInstrumentPlugInterfaceID = CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x8D, 0xC7, 0xC5, 0x82, 0x1C, 0x4B, 0x4F, 0x3C, 0xBE, 0xC8, 0x05, 0xCF, 0x83, 0x23, 0xCE, 0xA4)!
public let kPlayerPRODigitalPlugTypeID = CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xE9, 0xE5, 0x57, 0x4F, 0x50, 0xB4, 0x43, 0xE0, 0x94, 0x8D, 0x8B, 0x7C, 0x80, 0xD4, 0x72, 0x61)!
public let kPlayerPRODigitalPlugInterfaceID = CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x34, 0xBA, 0x67, 0x5D, 0x3E, 0xD8, 0x49, 0xF9, 0x8D, 0x06, 0x28, 0xA7, 0x43, 0x6A, 0x0E, 0x4D)!
