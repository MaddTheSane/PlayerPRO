//
//  SamplePlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 2/10/15.
//
//

import Cocoa
import PlayerPROKit

class SamplePlugHandler: NSObject {
	private(set) var plugIns = [PPSamplePlugObject]()
	
	override init() {
		let defaultPlugLocs = swiftDefaultPlugInLocations()
		var defaultManager = NSFileManager.defaultManager()
		
		for url in defaultPlugLocs {
			if let components = defaultManager.contentsOfDirectoryAtURL(url, includingPropertiesForKeys: [], options: nil, error: nil) as [NSURL]? {
				for component in components {
					if let ext = component.pathExtension {
						if ext.compare("plugin", options: .CaseInsensitiveSearch) != .OrderedSame {
							continue
						}
						let theBundle = NSBundle(URL: component)
						let aClass: AnyClass? = theBundle?.principalClass
						if aClass == nil {
							continue
						}
						
						if let aPlug = PPSamplePlugObject(bundle: theBundle) {
							plugIns.append(aPlug)
						}
					}
				}
			}
		}
		
		super.init()
	}
	
	var count: Int {
		return plugIns.count
	}
	
	func generate() -> IndexingGenerator<[PPSamplePlugObject]> {
		return plugIns.generate()
	}
	
	@objc subscript (index: Int) -> PPSamplePlugObject {
		return plugIns[index]
	}
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (plugIns as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
}
