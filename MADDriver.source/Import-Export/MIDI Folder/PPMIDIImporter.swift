//
//  PPMIDIImporter.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa
import PlayerPROKit

@objc(PPMIDIImporter) final public class MIDIImporter: NSObject, PPComplexImportPlugInterface {

	public func beginImportOfURL(theURL: NSURL, withHandler: PPComplexImportHandler?) {
		
	}
	
	public func canImportURL(theURL: NSURL!, error outErr: NSErrorPointer) -> Bool {
		return false
	}
	
	public override init() {
		
		super.init()
	}
	
	public func canImportURL(theURL: NSURL!) -> Bool {
		return canImportURL(theURL, error:nil)
	}
}
