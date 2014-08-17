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

extension MADFourChar: /*Printable, DebugPrintable,*/ StringLiteralConvertible {
	public var stringValue: String {
		get {
			let toRet = UTCreateStringForOSType(self as OSType).takeRetainedValue()
			return toRet as NSString
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

#if false
extension PPInfoRec: DebugPrintable {
	
	public var debugDescription: String { get {
		return ""
		}}
}
	#endif

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

extension Pcmd {
	public mutating func getCommand(row: Int16, track: Int16) -> UnsafeMutablePointer<Cmd> {
		return MADGetCmd(row, track, &self)
	}
}

//#pragma mark Plug-in functions

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

