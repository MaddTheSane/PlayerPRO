//
//  ComplexImportPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/17/14.
//
//

import Cocoa
import PlayerPROKit

final class ComplexImportPlugHandler: NSObject, NSFastEnumeration, CollectionType {
	
	private(set) var plugIns = [PPComplexImportPlugObject]()
	
	var startIndex: Int {
		return 0
	}
	
	var endIndex: Int {
		return plugIns.count
	}
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations() 
		let defaultManager = NSFileManager.defaultManager()
		
		for url in defaultPlugLocs {
			do {
				let components = try defaultManager.contentsOfDirectoryAtURL(url, includingPropertiesForKeys: [], options: [])
				let aComp = components.filter({ (aURL) -> Bool in
					if let ext = aURL.pathExtension {
						if ext.compare("ppextimp", options: .CaseInsensitiveSearch) == .OrderedSame {
							return true
						}
					}
					
					return false
				})
				
				for component in aComp {
					if let theBundle = NSBundle(URL: component), aPlug = PPComplexImportPlugObject(bundle: theBundle) {
						plugIns.append(aPlug)
					}
				}
			} catch {
				
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
