//
//  DigitalPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/19/14.
//
//

import Cocoa
import PlayerPROKit

final class DigitalPlugHandler: NSObject, NSFastEnumeration, Collection {
	private(set) var digitalPlugs = [DigitalPlugInObject]()
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations()
		let defaultManager = FileManager.default
		for aPlugLoc in defaultPlugLocs {
			do {
				let components = try defaultManager.contentsOfDirectory(at: aPlugLoc, includingPropertiesForKeys: [], options: [])
				let aComp = components.filter({ (aURL) -> Bool in
					if let ext = aURL.pathExtension {
						if ext.compare("plugin", options: .caseInsensitive) == .orderedSame {
							return true
						}
					}
					
					return false
				})
				
				
				for component in aComp {
					if let theBundle = Bundle(url: component), let tempObj = DigitalPlugInObject(bundle: theBundle) {
						digitalPlugs.append(tempObj)
					}
				}
			} catch _ { }
		}
		super.init()
	}
	
	func beginCallDigitalPlugIn(_ plugNum: Int, pcmd myPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver, parentDocument doc: PPDocument, handler: PPPlugErrorBlock) {
		let tmp = digitalPlugs[plugNum];
		tmp.beginCall(with: myPcmd, driver: driver, parentDocument: doc, handler: handler)
	}
	
	func addPlugInFromBundle(_ theBund: Bundle) {
		if let obj = DigitalPlugInObject(bundle: theBund) {
			digitalPlugs.append(obj)
		}
	}
	
	func addPlugInFromURL(_ urlpath: URL) {
		if let theBund = Bundle(url: urlpath) {
			addPlugInFromBundle(theBund)
		}
	}
	
	func addPlugInFromPath(_ thePath: String) {
		if let theBund = Bundle(path: thePath) {
			addPlugInFromBundle(theBund)
		}
	}
	
	var plugInCount: Int {
		return self.count
	}
	
	func makeIterator() -> IndexingIterator<[DigitalPlugInObject]> {
		return digitalPlugs.makeIterator()
	}
	
	@nonobjc var startIndex: Int {
		return digitalPlugs.startIndex
	}
	
	@nonobjc var endIndex: Int {
		return digitalPlugs.endIndex
	}
	
	@nonobjc func index(after i: Int) -> Int {
		return i + 1
	}
	
	subscript (subRange: Range<Int>) -> ArraySlice<DigitalPlugInObject> {
		return digitalPlugs[subRange]
	}
	
	@objc subscript (index: Int) -> DigitalPlugInObject {
		return digitalPlugs[index]
	}
	
	func countByEnumerating(with state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>!, count len: Int) -> Int {
		return (digitalPlugs as NSArray).countByEnumerating(with: state, objects: buffer, count: len)
	}
}
