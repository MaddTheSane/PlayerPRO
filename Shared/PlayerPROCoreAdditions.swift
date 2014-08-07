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

extension MADFourChar: Printable {
	public var stringValue: String {
	get {
		let toRet = UTCreateStringForOSType(self as OSType).takeRetainedValue()
		return toRet as NSString
	}
	}
	
	init(_ toInit: String) {
		self = UTGetOSTypeFromString(toInit as NSString as CFStringRef)
	}
	
	static func convertFromStringLiteral(value: String) -> MADFourChar {
		return MADFourChar(value)
	}
	
	public var description: String { get {
		return self.stringValue
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


@prefix func !(a: MADBool) -> MADBool {
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

extension MADBool : BooleanLiteralConvertible, LogicValue {
	init(_ v : LogicValue) {
		if v.getLogicValue() {
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
	
	public func getLogicValue() -> Bool {
		if (self == 0) {
			return false
		} else {
			return true
		}
	}
}
