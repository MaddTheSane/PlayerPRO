//
//  DigitalPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/19/14.
//
//

import Cocoa
import PlayerPROKit

func swiftDefaultPlugInLocations() -> [NSURL] {
	return DefaultPlugInLocations() as [NSURL]
}

class DigitalPlugHandler: NSObject, NSFastEnumeration, SequenceType, Sliceable {
	private var digitalPlugs = [PPDigitalPlugInObject]()
	
	override init() {
		let defaultPlugLocs = swiftDefaultPlugInLocations()
		for aPlugLoc in defaultPlugLocs {
			let somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, aPlugLoc, "plugin")
			if (somePlugs != nil) {
				for var i = 0; i < CFArrayGetCount(somePlugs); i++ {
					var tempCFBundle = unsafeBitCast(CFArrayGetValueAtIndex(somePlugs, i), CFBundle.self)
					let tempBundle = NSBundle(URL: CFBundleCopyBundleURL(tempCFBundle));
					var tempObj:PPDigitalPlugInObject? = PPDigitalPlugInObject.createWithBundle(tempBundle) //For some odd reason, the init function doesn't return PPDigitalPlugInObject!
					if let foo = tempObj? {
						digitalPlugs.append(foo)
					}
				}
			}
		}
		super.init()
	}
	
	func callDigitalPlugIn(plugNum: Int, pcmd myPcmd:UnsafeMutablePointer<Pcmd>, inout info theInfo: PPInfoPlug) -> MADErr {
		theInfo.fileType = "PPDG";
		let tmp = digitalPlugs[plugNum];
		return tmp.callWithPcmd(myPcmd, plugInfo: &theInfo)
	}
	
	func callDigitalPlugIn(plugNum: Int, pcmd myPcmd:UnsafeMutablePointer<Pcmd>, plugInfo theInfo:UnsafeMutablePointer<PPInfoPlug>) -> MADErr {
		theInfo.memory.fileType = "PPDG";
		let tmp = digitalPlugs[plugNum];
		return tmp.callWithPcmd(myPcmd, plugInfo: theInfo)
	}
	
	func addPlugInFromBundle(theBund: NSBundle) {
		let obj = PPDigitalPlugInObject.createWithBundle(theBund) //For some odd reason, the init function doesn't return PPDigitalPlugInObject!
		if (obj != nil) {
			digitalPlugs.append(obj)
		}
	}
	
	func addPlugInFromURL(urlpath: NSURL) {
		let theBund = NSBundle(URL: urlpath)
		addPlugInFromBundle(theBund)
	}
	
	func addPlugInFromPath(thePath: String) {
		let theBund = NSBundle(path: thePath);
		addPlugInFromBundle(theBund)
	}
	
	func generate() -> IndexingGenerator<[PPDigitalPlugInObject]> {
		return digitalPlugs.generate()
	}
	
	var startIndex: Int { get {
		return digitalPlugs.startIndex
		}}
	
	var endIndex: Int { get {
		return digitalPlugs.endIndex
		}}
	
	subscript (subRange: Range<Int>) -> Slice<PPDigitalPlugInObject> {
		return digitalPlugs[subRange]
	}
	
	@objc subscript (index: Int) -> PPDigitalPlugInObject {
		return digitalPlugs[index]
	}
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (digitalPlugs as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
}
