//
//  ColorPreferenceObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 12/28/14.
//
//

import Cocoa

@objc(PPColorPreferenceObject) class ColorPreferenceObject: NSObject {
	var color: NSColor
	let colorIndex: Int
	
	init(Color aColor:NSColor, index idx: Int) {
		color = aColor
		colorIndex = idx
		
		super.init()
	}
}
