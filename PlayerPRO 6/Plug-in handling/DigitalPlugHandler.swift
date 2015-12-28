//
//  DigitalPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/19/14.
//
//

import Cocoa
import PlayerPROKit

final class DigitalPlugHandler: NSObject, NSFastEnumeration, CollectionType {
	private(set) var digitalPlugs = [DigitalPlugInObject]()
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations()
		let defaultManager = NSFileManager.defaultManager()
		for aPlugLoc in defaultPlugLocs {
			do {
				let components = try defaultManager.contentsOfDirectoryAtURL(aPlugLoc, includingPropertiesForKeys: [], options: [])
				let aComp = components.filter({ (aURL) -> Bool in
					if let ext = aURL.pathExtension {
						if ext.compare("plugin", options: .CaseInsensitiveSearch) == .OrderedSame {
							return true
						}
					}
					
					return false
				})
				
				
				for component in aComp {
					if let theBundle = NSBundle(URL: component), tempObj = DigitalPlugInObject(bundle: theBundle) {
						digitalPlugs.append(tempObj)
					}
				}
			} catch _ { }
		}
		super.init()
	}
	
	func beginCallDigitalPlugIn(plugNum: Int, pcmd myPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver, parentDocument doc: PPDocument, handler: PPPlugErrorBlock) {
		let tmp = digitalPlugs[plugNum];
		tmp.beginCallWithPcmd(myPcmd, driver: driver, parentDocument: doc, handler: handler)
	}
	
	func addPlugInFromBundle(theBund: NSBundle) {
		if let obj = DigitalPlugInObject(bundle: theBund) {
			digitalPlugs.append(obj)
		}
	}
	
	func addPlugInFromURL(urlpath: NSURL) {
		if let theBund = NSBundle(URL: urlpath) {
			addPlugInFromBundle(theBund)
		}
	}
	
	func addPlugInFromPath(thePath: String) {
		if let theBund = NSBundle(path: thePath) {
			addPlugInFromBundle(theBund)
		}
	}
	
	var plugInCount: Int {
		return self.count
	}
	
	func generate() -> IndexingGenerator<[DigitalPlugInObject]> {
		return digitalPlugs.generate()
	}
	
	@nonobjc var startIndex: Int {
		return digitalPlugs.startIndex
	}
	
	@nonobjc var endIndex: Int {
		return digitalPlugs.endIndex
	}
	
	subscript (subRange: Range<Int>) -> ArraySlice<DigitalPlugInObject> {
		return digitalPlugs[subRange]
	}
	
	@objc subscript (index: Int) -> DigitalPlugInObject {
		return digitalPlugs[index]
	}
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (digitalPlugs as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
}
