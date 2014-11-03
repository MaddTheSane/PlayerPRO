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
