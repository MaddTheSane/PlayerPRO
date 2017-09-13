//
//  ComplexImportPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/17/14.
//
//

import Cocoa
import PlayerPROKit

final class ComplexImportPlugHandler: NSObject, NSFastEnumeration, Collection {
	typealias Index = Int
	private(set) var plugIns = [PPComplexImportPlugObject]()
	
	@nonobjc var startIndex: Int {
		return 0
	}
	
	@nonobjc var endIndex: Int {
		return plugIns.count
	}
	
	@nonobjc func index(after i: ComplexImportPlugHandler.Index) -> ComplexImportPlugHandler.Index {
		return i + 1
	}
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations() 
		let defaultManager = FileManager.default
		
		for url in defaultPlugLocs {
			do {
				let components = try defaultManager.contentsOfDirectory(at: url, includingPropertiesForKeys: [], options: [])
				let aComp = components.filter({ (aURL) -> Bool in
					let ext = aURL.pathExtension
					if ext.compare("ppextimp", options: .caseInsensitive) == .orderedSame {
						return true
					}
					
					return false
				})
				
				for component in aComp {
					if let theBundle = Bundle(url: component), let aPlug = PPComplexImportPlugObject(bundle: theBundle) {
						plugIns.append(aPlug)
					}
				}
			} catch {
				
			}
		}
		
		super.init()
	}
	
	var count: Int {
		return plugIns.count
	}
	
	var plugInCount: Int {
		return self.count
	}
	
	func makeIterator() -> IndexingIterator<[PPComplexImportPlugObject]> {
		return plugIns.makeIterator()
	}
	
	@objc subscript (index: Int) -> PPComplexImportPlugObject {
		return plugIns[index]
	}
	
	func countByEnumerating(with state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (plugIns as NSArray).countByEnumerating(with: state, objects: buffer, count: len)
	}
}
