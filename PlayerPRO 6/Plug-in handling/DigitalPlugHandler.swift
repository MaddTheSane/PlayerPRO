//
//  DigitalPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/19/14.
//
//

import Cocoa
import PlayerPROKit

class DigitalPlugHandler: NSObject, NSFastEnumeration {
	var digitalPlugs = [PPDigitalPlugInObject]()
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations() as [NSURL]
		for aPlugLoc in defaultPlugLocs {
			let somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, aPlugLoc, "plugin")
			let someCFPlugs = unsafeBitCast(somePlugs, NSArray.self) as [CFBundle]
			for tempCFBundle in someCFPlugs {
				let tempBundle = NSBundle(URL:CFBundleCopyBundleURL(tempCFBundle));
				PPDigitalPlugInObject()
				var tempObj = PPDigitalPlugInObject(bundle: tempBundle)
				//digitalPlugs.append(tempObj?)
			}
		}
		
		
		super.init()
	}
	
	
	
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (digitalPlugs as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
}
