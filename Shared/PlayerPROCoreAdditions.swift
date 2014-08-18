//
//  PlayerPROCoreAdditions.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Foundation
import PlayerPROCore

#if os(OSX)
import CoreServices

let MadID: MADFourChar = "MADK"

extension MADFourChar: /*Printable, DebugPrintable,*/ StringLiteralConvertible {
	public var stringValue: String {
		get {
			let toRet = UTCreateStringForOSType(self as OSType).takeRetainedValue()
			return toRet as NSString as String
		}}
	
	public init(_ toInit: String) {
		self = UTGetOSTypeFromString(toInit as NSString as CFString)
	}
	
	#if false
	public init(_ toInit: (Int8, Int8, Int8, Int8, Int8)) {
		var tmpInit = toInit
		var atmp = &tmpInit
		var tmpPtr: UnsafeMutablePointer<Int8> = UnsafeMutablePointer<Int8>(tmpInit)
		self = Ptr2OSType(tmpInit)
	}
	
	public var description: String { get {
		return self.stringValue
		}}
	
	public var debugDescription: String { get {
		return self.description
		}}
	#endif
	
	public static func convertFromStringLiteral(value: String) -> MADFourChar {
		return MADFourChar(value)
	}
	
	public static func convertFromExtendedGraphemeClusterLiteral(value: String) -> MADFourChar {
		var tmpStr = String.convertFromExtendedGraphemeClusterLiteral(value)
		return self.convertFromStringLiteral(tmpStr)
	}
	
}
#endif

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
		MADGetBestDriver(&self)
	}
	
	public var debugDescription: String { get {
		let onVal = "on"
		let offVal = "off"
		return "Driver Mode: \(driverMode), output mode: \(outPutMode); Channel count: \(numChn), output Rate: \(outPutRate), surround: \(surround == true ? onVal : offVal); micro-delay size: \(MicroDelaySize), reverb, is \(Reverb == true ? onVal: offVal), size: \(ReverbSize), strength: \(ReverbStrength); oversampling \(oversampling); repeat music: \(repeatMusic == true ? onVal : offVal); "
		}}
}

#if false
func &(lhs: MADBool, rhs: MADBool) -> MADBool {
	if lhs {
		return rhs
	}
	return false
}

func |(lhs: MADBool, rhs: MADBool) -> MADBool {
	if lhs {
		return true
	}
	return rhs
}

func ^(lhs: MADBool, rhs: MADBool) -> MADBool {
	return MADBool(lhs != rhs)
}


prefix func !(a: MADBool) -> MADBool {
	return a ^ true
}

func &=(inout lhs: MADBool, rhs: MADBool) {
	var lhsB: MADBool = 0
	var rhsB: MADBool = 0
	if lhs {
		lhsB = 1
	}
	if rhs {
		rhsB = 1
	}
	lhs = lhsB & rhsB
}
#endif

extension PPInfoRec: DebugPrintable {
	
	public var debugDescription: String { get {
		return ""
		}}
}

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
		self.name.0 = 0
		self.name.1 = 0
		self.name.2 = 0
		self.name.3 = 0
		self.name.4 = 0
		self.name.5 = 0
		self.name.6 = 0
		self.name.7 = 0
		self.name.8 = 0
		self.name.9 = 0
		self.name.10 = 0
		self.name.11 = 0
		self.name.12 = 0
		self.name.13 = 0
		self.name.14 = 0
		self.name.15 = 0
		self.name.16 = 0
		self.name.17 = 0
		self.name.18 = 0
		self.name.19 = 0
		self.name.20 = 0
		self.name.21 = 0
		self.name.22 = 0
		self.name.23 = 0
		self.name.24 = 0
		self.name.25 = 0
		self.name.26 = 0
		self.name.27 = 0
		self.name.28 = 0
		self.name.29 = 0
		self.name.30 = 0
		self.name.31 = 0
		self.stereo = 0
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
		
		var toRetName = reflect(toRet.name)
		var ourName = reflect(self.name)
		for i in 0 ..< toRetName.count {
			//toRetName[i].0
		}
		toRet.stereo = self.stereo
		
		
		return toRet
		}}
}

private func iterate<C,R>(t:C, block:(String,Any)->R) {
	let mirror = reflect(t)
	for i in 0 ..< mirror.count {
		block(mirror[i].0, mirror[i].1.value)
	}
}


extension sData {
	public static let MaxVolume: MADByte = 64
	public static let NoFineTune: UInt16 = 8363

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
		
		var toRetName = reflect(toRet.name)
		var ourName = reflect(self.name)
		for i in 0 ..< toRetName.count {
			//toRetName[i].0
		}
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
		self.name.0 = 0
		self.name.1 = 0
		self.name.2 = 0
		self.name.3 = 0
		self.name.4 = 0
		self.name.5 = 0
		self.name.6 = 0
		self.name.7 = 0
		self.name.8 = 0
		self.name.9 = 0
		self.name.10 = 0
		self.name.11 = 0
		self.name.12 = 0
		self.name.13 = 0
		self.name.14 = 0
		self.name.15 = 0
		self.name.16 = 0
		self.name.17 = 0
		self.name.18 = 0
		self.name.19 = 0
		self.name.20 = 0
		self.name.21 = 0
		self.name.22 = 0
		self.name.23 = 0
		self.name.24 = 0
		self.name.25 = 0
		self.name.26 = 0
		self.name.27 = 0
		self.name.28 = 0
		self.name.29 = 0
		self.name.30 = 0
		self.name.31 = 0
		self.stereo = 0
		self.data = nil
	}
}

extension MADBool : BooleanLiteralConvertible, BooleanType {
	//public typealias BooleanLiteralType = Bool
	public init(_ v : BooleanType) {
		if v.boolValue {
			self = 1
		} else {
			self = 0
		}
	}
	
	public static func convertFromBooleanLiteral(value: BooleanLiteralType) -> MADBool {
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

extension Cmd {
	public init() {
		ins = 0
		note = 0xff
		cmd = 0
		arg = 0
		vol = 0xff
		unused = 0
	}
	
	public mutating func kill() -> MADErr {
		return MADKillCmd(&self)
	}
}

extension PatData {
	public mutating func getMADCommand(position: Int16, channel: Int16) -> UnsafeMutablePointer<Cmd> {
		return GetMADCommand(position, channel, &self)
	}
}

//#pragma mark Plug-in functions

extension Pcmd {
	public mutating func getCommand(row: Int16, track: Int16) -> UnsafeMutablePointer<Cmd> {
		return MADGetCmd(row, track, &self)
	}
}

var kPlayerPROFiltersPlugTypeID: CFUUID { get {
	return CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x79, 0xEA, 0x82, 0xAD, 0x5A, 0x53, 0x46, 0xAF, 0x82, 0xA9, 0x4A, 0x06, 0x85, 0xB4, 0x58, 0x8C)
}}

var kPlayerPROFiltersPlugInterfaceID: CFUUID { get {
	return CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xDA, 0x70, 0x82, 0xA2, 0xFE, 0xF1, 0x44, 0x75, 0xB1, 0xA4, 0x35, 0xC8, 0x1E, 0xD5, 0xDB, 0x8F)
}}

var kPlayerPROInstrumentPlugTypeID: CFUUID { get{
	return CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xFD, 0x71, 0x54, 0xD6, 0x20, 0xBF, 0x40, 0x07, 0x88, 0x1B, 0x8E, 0x44, 0x97, 0x0C, 0x3B, 0x0A)
}}

var kPlayerPROInstrumentPlugInterfaceID: CFUUID { get{
	return CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x8D, 0xC7, 0xC5, 0x82, 0x1C, 0x4B, 0x4F, 0x3C, 0xBE, 0xC8, 0x05, 0xCF, 0x83, 0x23, 0xCE, 0xA4)
}}

var kPlayerPRODigitalPlugTypeID: CFUUID { get{
	return CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xE9, 0xE5, 0x57, 0x4F, 0x50, 0xB4, 0x43, 0xE0, 0x94, 0x8D, 0x8B, 0x7C, 0x80, 0xD4, 0x72, 0x61)
}}

var kPlayerPRODigitalPlugInterfaceID: CFUUID { get{
	return CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x34, 0xBA, 0x67, 0x5D, 0x3E, 0xD8, 0x49, 0xF9, 0x8D, 0x06, 0x28, 0xA7, 0x43, 0x6A, 0x0E, 0x4D)
}}

