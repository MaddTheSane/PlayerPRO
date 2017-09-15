//
//  FilterPlugHandle.swift
//  PPMacho
//
//  Created by C.W. Betts on 12/28/15.
//
//

import Foundation
import PlayerPROCore

class FilterPlugHandler: NSObject, NSFastEnumeration, Collection, Sequence {
	typealias Index = Int
	private(set) var plugInArray = [PPFilterPlugObject]()
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations()
		let defaultManager = FileManager.default
		for aPlugLoc in defaultPlugLocs {
			do {
				let components = try defaultManager.contentsOfDirectory(at: aPlugLoc, includingPropertiesForKeys: [], options: [])
				let aComp = components.filter({ (aURL) -> Bool in
					let ext = aURL.pathExtension
					if ext.compare("plugin", options: .caseInsensitive) == .orderedSame {
						return true
					}
					
					return false
				})
				
				
				for component in aComp {
					if let theBundle = Bundle(url: component),
						let tempObj = PPFilterPlugObject(bundle: theBundle) {
						plugInArray.append(tempObj)
					}
				}
			} catch _ { }
		}
		super.init()
	}
	
	subscript (subRange: Range<Int>) -> ArraySlice<PPFilterPlugObject> {
		return plugInArray[subRange]
	}
	
	subscript (index: Int) -> PPFilterPlugObject {
		return plugInArray[index]
	}
	
	func countByEnumerating(with state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (plugInArray as NSArray).countByEnumerating(with: state, objects: buffer, count: len)
	}
	
	var count: Int {
		return plugInArray.count
	}
	
	@nonobjc var startIndex: Int {
		return 0
	}
	
	@nonobjc var endIndex: Int {
		return plugInArray.count
	}
	
	@nonobjc func index(after i: FilterPlugHandler.Index) -> FilterPlugHandler.Index {
		return i + 1
	}
	
	var plugInCount: Int {
		return count
	}
	
	func makeIterator() -> IndexingIterator<[PPFilterPlugObject]> {
		return plugInArray.makeIterator();
	}

	func beginWithPlugAtIndex(_ idx: Int, data theData: PPSampleObject, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver, parentDocument document: NSDocument, handler: @escaping PPPlugErrorBlock) {
		let aPlug = plugInArray[idx]
		aPlug.beginRun(withData: theData, selectionRange: selRange, onlyCurrentChannel: StereoMode, driver: driver, parentDocument: document, handler: handler)
	}
	
	@objc(addPlugInFromPath:) func addPlugIn(fromPath thePath: String) {
		if let bund = Bundle(path: thePath) {
			addPlugIn(from: bund)
		}
	}
	
	@objc(addPlugInFromURL:) func addPlugIn(from urlpath: URL) {
		if let bund = Bundle(url: urlpath) {
			addPlugIn(from: bund)
		}
	}
	
	@objc(addPlugInFromBundle:) func addPlugIn(from theBund: Bundle) {
		if let newPlug = PPFilterPlugObject(bundle: theBund) {
			plugInArray.append(newPlug)
		}
	}
}
