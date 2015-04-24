//
//  DigitalPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/19/14.
//
//

import Cocoa
import PlayerPROKit

class DigitalPlugHandler: NSObject, NSFastEnumeration, CollectionType, Sliceable {
	private var digitalPlugs = [PPDigitalPlugInObject]()
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations() as! [NSURL]
		var defaultManager = NSFileManager.defaultManager()
		for aPlugLoc in defaultPlugLocs {
			if let components = defaultManager.contentsOfDirectoryAtURL(aPlugLoc, includingPropertiesForKeys: [], options: nil, error: nil) as? [NSURL] {
				let aComp = filter(components, { (aURL) -> Bool in
					if let ext = aURL.pathExtension {
						if ext.compare("plugin", options: .CaseInsensitiveSearch) == .OrderedSame {
							return true
						}
					}
					
					return false
				})
				
				
				for component in aComp {
					let theBundle = NSBundle(URL: component)
					if let tempObj = PPDigitalPlugInObject(bundle: theBundle) {
						digitalPlugs.append(tempObj)
					}
				}
			}
		}
		super.init()
	}
	
	func beginCallDigitalPlugIn(plugNum: Int, pcmd myPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver, parentDocument doc: PPDocument, handler: PPPlugErrorBlock) {
		let tmp = digitalPlugs[plugNum];
		tmp.beginCallWithPcmd(myPcmd, driver: driver, parentDocument: doc, handler: handler)
	}
	
	func addPlugInFromBundle(theBund: NSBundle) {
		if let obj = PPDigitalPlugInObject(bundle: theBund) as PPDigitalPlugInObject? {
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
	
	func generate() -> IndexingGenerator<[PPDigitalPlugInObject]> {
		return digitalPlugs.generate()
	}
	
	var startIndex: Int {
		return digitalPlugs.startIndex
	}
	
	var endIndex: Int {
		return digitalPlugs.endIndex
	}
	
	subscript (subRange: Range<Int>) -> ArraySlice<PPDigitalPlugInObject> {
		return digitalPlugs[subRange]
	}
	
	@objc subscript (index: Int) -> PPDigitalPlugInObject {
		return digitalPlugs[index]
	}
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (digitalPlugs as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
}
