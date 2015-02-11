//
//  ComplexImportPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/17/14.
//
//

import Cocoa
import PlayerPROKit

final class ComplexImportPlugHandler: NSObject, NSFastEnumeration, SequenceType {
	
	private(set) var plugIns = [PPComplexImportPlugObject]()
	
	override init() {
		let defaultPlugLocs = swiftDefaultPlugInLocations()
		var defaultManager = NSFileManager.defaultManager()
		
		for url in defaultPlugLocs {
			if let components = defaultManager.contentsOfDirectoryAtURL(url, includingPropertiesForKeys: [], options: nil, error: nil) as [NSURL]? {
				for component in components {
					if let ext = component.pathExtension {
						if ext.compare("ppextimp", options: .CaseInsensitiveSearch) != .OrderedSame {
							continue
						}
						let theBundle = NSBundle(URL: component)
						let aClass: AnyClass? = theBundle?.principalClass
						if aClass == nil {
							continue
						}
						
						if let aPlug = PPComplexImportPlugObject(bundle: theBundle) {
							plugIns.append(aPlug)
						}
					}
				}
			}
		}
		
		super.init()
	}
	
	var count: Int {
		get {
			return plugIns.count
		}
	}
	
	func generate() -> IndexingGenerator<[PPComplexImportPlugObject]> {
		return plugIns.generate()
	}
	
	@objc subscript (index: Int) -> PPComplexImportPlugObject {
		return plugIns[index]
	}
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (plugIns as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
	
}
