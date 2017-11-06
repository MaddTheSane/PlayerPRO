//
//  ColorPreferenceObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 12/28/14.
//
//

import Cocoa

class ColorPreferenceObject: NSObject {
	@objc var color: NSColor
	@objc let colorIndex: Int
	
	@objc(initWithColor:index:)
	init(color aColor:NSColor, index idx: Int) {
		color = aColor
		colorIndex = idx
		
		super.init()
	}
}
