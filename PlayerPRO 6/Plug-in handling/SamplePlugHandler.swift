//
//  SamplePlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 2/10/15.
//
//

import Cocoa
import PlayerPROKit

final class SamplePlugHandler: NSObject, CollectionType, NSFastEnumeration {
	private(set) var plugIns = [PPSamplePlugObject]()
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations()
		let defaultManager = NSFileManager.defaultManager()
		
		for url in defaultPlugLocs {
			do {
				let components = try defaultManager.contentsOfDirectoryAtURL(url, includingPropertiesForKeys: [], options: [])
				let aComp = components.filter({ (aURL) -> Bool in
					if let ext = aURL.pathExtension {
						if ext.compare("plugin", options: .CaseInsensitiveSearch) == .OrderedSame {
							return true
						}
					}
					
					return false
				})
				for component in aComp {
					if let theBundle = NSBundle(URL: component), aPlug = PPSamplePlugObject(bundle: theBundle) {
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
	
	@nonobjc var startIndex: Int {
		return 0
	}
	
	@nonobjc var endIndex: Int {
		return plugIns.count
	}
	
	func generate() -> IndexingGenerator<[PPSamplePlugObject]> {
		return plugIns.generate()
	}
	
	@objc subscript (index: Int) -> PPSamplePlugObject {
		return plugIns[index]
	}
	
	func beginExportingSample(sample: PPSampleObject, type: OSType, URL url: NSURL, driver: PPDriver, parentDocument document: PPDocument, handler: PPPlugErrorBlock) {
		var aPlug: PPSamplePlugObject? = nil
		
		for plug in plugIns {
			if plug.type == type {
				aPlug = plug
				break
			}
		}
		
		if let plug = aPlug {
			plug.beginExportSample(sample, toURL: url, driver: driver, parentDocument: document, handler: handler)
		} else {
			handler(.CannotFindPlug)
		}
	}
	
	func beginImportingSample(type type: OSType, URL url: NSURL, driver: PPDriver, parentDocument document: PPDocument, handler: (errorCode: MADErr, createdIns: PPSampleObject?) -> Void) {
		var aPlug: PPSamplePlugObject? = nil
		
		for plug in plugIns {
			if plug.type == type {
				aPlug = plug
				break
			}
		}
		
		if let plug = aPlug {
			plug.beginImportSampleAtURL(url, driver: driver, parentDocument: document, handler: handler)
		} else {
			handler(errorCode: .CannotFindPlug, createdIns: nil)
		}
	}
	
	func testSampleFile(toTest: NSURL, type theType: OSType) -> MADErr {
		for plug in plugIns {
			if plug.type == theType {
				return plug.canImportFileAtURL(toTest) ? .NoErr : .FileNotSupportedByThisPlug
			}
		}

		return .CannotFindPlug
	}
	
	func isPlugAvailable(kind: OSType) -> Bool {
		for plug in plugIns {
			if plug.type == kind {
				return true
			}
		}
		return false
	}
	
	func identifySampleFile(ref: NSURL) -> (error: MADErr, type: OSType) {
		for plug in plugIns {
			if plug.canImportFileAtURL(ref) {
				return (.NoErr, plug.type)
			}
		}
		
		return (.CannotFindPlug, "!!!!")
	}
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (plugIns as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
}
