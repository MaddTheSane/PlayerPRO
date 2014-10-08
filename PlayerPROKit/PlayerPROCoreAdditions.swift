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

internal func CFStringToString(cfStr: CFString) -> String {
	return cfStr as NSString as String
}

internal func StringToCFString(string: String) -> CFString {
	return string as NSString as CFString
}

// MARK: Bridges to more modern Swift code.
public let MadID = StringToOSType("MADK")

#if os(OSX)
import CoreServices

public let PlugMenuNameKey = CFStringToString(kMadPlugMenuNameKey)
public let PlugAuthorNameKey = CFStringToString(kMadPlugAuthorNameKey)
public let PlugUTITypesKey = CFStringToString(kMadPlugUTITypesKey)
public let PlugTypeKey = CFStringToString(kMadPlugTypeKey)
public let PlugDoesImport = CFStringToString(kMadPlugDoesImport)
public let PlugDoesExport = CFStringToString(kMadPlugDoesExport)
public let PlugModeKey = CFStringToString(kMadPlugModeKey)

public func OSTypeToString(theType: MADFourChar) -> String? {
	let toRet = UTCreateStringForOSType(theType)
	if toRet == nil {
		return nil
	}
	return CFStringToString(toRet.takeRetainedValue())
}

public func StringToOSType(theString: String) -> MADFourChar {
	return UTGetOSTypeFromString(StringToCFString(theString))
}

/*
	extension MADFourChar: StringLiteralConvertible {
	public static func convertFromStringLiteral(value: String) -> MADFourChar {
		return MADFourChar(value)
	}
	
	public static func convertFromExtendedGraphemeClusterLiteral(value: String) -> MADFourChar {
		var tmpStr = String.convertFromExtendedGraphemeClusterLiteral(value)
		return self.convertFromStringLiteral(tmpStr)
	}
	
}
*/
#else
	public func OSTypeToString(theType: MADFourChar) -> String? {
		var ourOSType = [Int8](count: 5, repeatedValue: 0)
	
		OSType2Ptr(theType, &ourOSType)
		return NSString(bytes: ourOSType, length: 4, encoding: NSMacOSRomanStringEncoding);
	}
	
	public func StringToOSType(theString: String) -> MADFourChar {
		var ourOSType = [Int8](count: 5, repeatedValue: 0)
		let anNSStr = theString as NSString
		var ourLen = anNSStr.lengthOfBytesUsingEncoding(NSMacOSRomanStringEncoding)
		if ourLen > 4 {
			ourLen = 4
		} else if ourLen == 0 {
			return 0
		}
		
		let aData = anNSStr.cStringUsingEncoding(NSMacOSRomanStringEncoding)
		
		for i in 0 ..< ourLen {
			ourOSType[i] = aData[i]
		}
		
		return Ptr2OSType(ourOSType)
	}
#endif

extension MADFourChar {
	public init(_ toInit: String) {
		self = StringToOSType(toInit)
	}
	
	public init(_ toInit: (Int8, Int8, Int8, Int8, Int8)) {
		self = MADFourChar((toInit.0, toInit.1, toInit.2, toInit.3))
	}
	
	public var OSTypeStringValue: String? {
		get {
			return OSTypeToString(self)
		}
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
}

// MARK: PlayerPRO MAD data types

extension MADDriverSettings: DebugPrintable {
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
		var toParse = [CChar]()
		var mirror = reflect(internalFileName)
		for i in 0..<mirror.count {
			var aChar = mirror[i].1.value as CChar
			toParse.append(aChar)
		}
		return NSString(CString: toParse, encoding: NSMacOSRomanStringEncoding)
	}
	
	public var format: String! {
		var toParse = [CChar]()
		var mirror = reflect(formatDescription)
		for i in 0..<mirror.count {
			var aChar = mirror[i].1.value as CChar
			toParse.append(aChar)
		}
		return NSString(CString: toParse, encoding: NSMacOSRomanStringEncoding)
	}
	
	public var debugDescription: String {
		get {
			return "\(internalName), format \(format)"
		}
	}
}

extension PlugInfo {
	public var importer: Bool {
		get {
			switch (self.mode) {
			case MADPlugModes.Import.rawValue, MADPlugModes.ImportExport.rawValue:
				return true

			default:
				return false
			}
		}
	}

	public var exporter: Bool {
		get {
			switch (self.mode) {
			case MADPlugModes.Export.rawValue, MADPlugModes.ImportExport.rawValue:
				return true

			default:
				return false
			}
		}
	}
	
	public var plugInURL: NSURL {
		get {
			return CFBundleCopyBundleURL(file.takeUnretainedValue()) as NSURL
		}
	}
	
	public var types: [String] {
		get {
			return UTItypes.takeUnretainedValue() as NSArray as [String]
		}
	}
	
	public var fourCharType: MADFourChar {
		get {
			return MADFourChar(type)
		}
	}
	
	public var name: String {
		get {
			return CFStringToString(MenuName.takeUnretainedValue())
		}
	}
	
	public var author: String {
		get {
			return CFStringToString(AuthorString.takeUnretainedValue())
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
		maxPlugs = library.count
	}
}

extension MADLibrary: SequenceType {
	public var count: Int {
		get {
			return Int(TotalPlug)
		}
	}
	
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
		self.vol = sData.maximumVolume
		self.c2spd = sData.noFineTune
		self.loopType = .Classic
		self.amp = 8
		self.relNote = 0
		self.name = BlankNameChar32
		self.stereo = false
		self.data = 0
	}
	
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
	public static let noFineTune: UInt16 = 8363
	public static let minimumVolume: MADByte = 0
	public static let maximumVolume: MADByte = 64

	public var toSData32 : sData32 {
		get {
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

extension EnvRec {
	public init() {
		pos = 0
		val = 0
	}
}

extension FXBus {
	public init() {
		Active = false
		ByPass = false
		copyId = 0
	}
}

extension InstrData {
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
	return GetCommand(position, channel, aPat).memory
}

internal func GetCommand(position: Int16, channel: Int16, aPat: UnsafeMutablePointer<PatData>) -> UnsafeMutablePointer<Cmd> {
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

// MARK: Plug-in functions

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
