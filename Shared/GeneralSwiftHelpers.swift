//
//  GeneralSwiftHelpers.swift
//  PPMacho
//
//  Created by C.W. Betts on 10/27/14.
//
//

import Foundation
import PlayerPROCore

internal func GetArrayFromMirror<X>(mirror: MirrorType) -> [X] {
	var anArray = [X]()
	for i in 0..<mirror.count {
		var aChar = mirror[i].1.value as X
		anArray.append(aChar)
	}
	
	return anArray
}

internal func GetArrayFromMirror<X>(mirror: MirrorType, appendLastObject lastObj: X) -> [X] {
	var anArray: [X] = GetArrayFromMirror(mirror)
	anArray.append(lastObj)
	return anArray
}

internal func CFStringToString(cfStr: CFString) -> String {
	return cfStr as NSString as String
}

internal func StringToCFString(string: String) -> CFString {
	return string as NSString as CFString
}

#if os(OSX)
import CoreServices

internal func OSTypeToString(theType: MADFourChar) -> String? {
	if let toRet = UTCreateStringForOSType(theType) {
		return CFStringToString(toRet.takeRetainedValue())
	} else {
		return nil
	}
}

internal func StringToOSType(theString: String) -> MADFourChar {
	return UTGetOSTypeFromString(StringToCFString(theString))
}
#else
internal func OSTypeToString(theType: MADFourChar) -> String? {
	var ourOSType = [Int8](count: 5, repeatedValue: 0)

	OSType2Ptr(theType, &ourOSType)
	return NSString(bytes: ourOSType, length: 4, encoding: NSMacOSRomanStringEncoding);
}

internal func StringToOSType(theString: String) -> MADFourChar {
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

