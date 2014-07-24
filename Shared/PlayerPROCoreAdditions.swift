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

extension MADFourChar {
	var stringValue: String {
	get {
		let toRet = UTCreateStringForOSType(self as OSType).takeRetainedValue()
		return toRet as NSString
	}
	}
	init(_ toInit: String) {
		self = UTGetOSTypeFromString(toInit as NSString as CFStringRef)
	}
}

extension MADBool : BooleanLiteralConvertible {
	public static func convertFromBooleanLiteral(value: BooleanLiteralType) -> MADBool {
		if (value == true) {
			return 1
		} else {
			return 0
		}
	}
}

extension MADBool : LogicValue {
	public func getLogicValue() -> Bool {
		if (self == 0) {
			return false
		} else {
			return true
		}
	}
}

