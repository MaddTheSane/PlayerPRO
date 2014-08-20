//
//  ComplexImportPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/17/14.
//
//

import Cocoa
import PlayerPROKit

final class ComplexImportPlugHandler: NSObject, NSFastEnumeration {
	
	var plugIns = [PPComplexImportPlugObject]()
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations() as [NSURL]
		var defaultManager = NSFileManager.defaultManager()
		
		for url in defaultPlugLocs {
			var components = defaultManager.contentsOfDirectoryAtURL(url, includingPropertiesForKeys: [], options: NSDirectoryEnumerationOptions.fromRaw(0)!, error: nil) as [NSURL]
			for component in components {
				if component.pathExtension != "ppextimp" {
					continue
				}
				var theBundle = NSBundle(URL: component);
				var aPlug = PPComplexImportPlugObject(bundle: theBundle)
			}
		}
		
		super.init()
	}
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (plugIns as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
	
}
