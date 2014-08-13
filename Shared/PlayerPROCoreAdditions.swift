//
//  PlayerPROCoreAdditions.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Foundation
import CoreServices
import PlayerPROCore

#if os(OSX)
extension MADFourChar: Printable, DebugPrintable, StringLiteralConvertible {
	public var stringValue: String {
		get {
			let toRet = UTCreateStringForOSType(self as OSType).takeRetainedValue()
			return toRet as NSString
		}}
	
	init(_ toInit: String) {
		self = UTGetOSTypeFromString(toInit as NSString as CFString)
	}
	
	/*
	init(_ toInit: (Int8, Int8, Int8, Int8, Int8)) {
		var tmpInit = toInit
		var atmp = &tmpInit
		var tmpPtr: UnsafeMutablePointer<Int8> = UnsafeMutablePointer<Int8>(tmpInit)
		self = Ptr2OSType(tmpInit)
	}
	*/
	
	public static func convertFromStringLiteral(value: String) -> MADFourChar {
		return MADFourChar(value)
	}
	
	public var description: String { get {
		return self.stringValue
		}}
	
	public var debugDescription: String { get {
		return self.description
		}}
	
	public static func convertFromExtendedGraphemeClusterLiteral(value: String) -> MADFourChar {
		var tmpStr = String.convertFromExtendedGraphemeClusterLiteral(value)
		return self.convertFromStringLiteral(tmpStr)
	}
	
}
#endif

extension MADDriverSettings: DebugPrintable {
	init() {
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

extension MADBool : BooleanLiteralConvertible, BooleanType {
	init(_ v : BooleanType) {
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
