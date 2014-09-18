//
//  TypeCount.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/18/14.
//
//

import Cocoa
import PlayerPROKit

class TypeCount: NSObject {
	let resourceType: String
	let resourceCount: Int
	
	init(type: String, count: Int) {
		resourceCount = count
		resourceType = type
		
		super.init()
	}
}
