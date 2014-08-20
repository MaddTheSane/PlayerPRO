//
//  DigitalPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/19/14.
//
//

import Cocoa
import PlayerPROKit

class DigitalPlugHandler: NSObject, NSFastEnumeration, SequenceType, Sliceable {
	private var digitalPlugs = [PPDigitalPlugInObject]()
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations() as [NSURL]
		super.init()
		for aPlugLoc in defaultPlugLocs {
			let somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, aPlugLoc, "plugin")
			let someCFPlugs = unsafeBitCast(somePlugs, NSArray.self) as [CFBundle]
			for tempCFBundle in someCFPlugs {
				autoreleasepool {
					let tempBundle = NSBundle(URL:CFBundleCopyBundleURL(tempCFBundle));
					var tempObj:PPDigitalPlugInObject? = PPDigitalPlugInObject.createWithBundle(tempBundle) //For some odd reason, the init function doesn't return PPDigitalPlugInObject!
					if let foo = tempObj? {
						self.digitalPlugs.append(foo)
					}
				}
			}
		}
	}
	
	func callDigitalPlugIn(plugNum: Int, pcmd myPcmd:UnsafeMutablePointer<Pcmd>, plugInfo theInfo:UnsafeMutablePointer<PPInfoPlug>) -> MADErr {
		theInfo.memory.fileType = "PPDG";
		var tmp = digitalPlugs[plugNum];
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
