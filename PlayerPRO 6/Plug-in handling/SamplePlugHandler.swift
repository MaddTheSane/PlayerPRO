//
//  SamplePlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 2/10/15.
//
//

import Cocoa
import PlayerPROKit

class SamplePlugHandler: NSObject, SequenceType, NSFastEnumeration {
	private(set) var plugIns = [PPSamplePlugObject]()
	
	override init() {
		let defaultPlugLocs = swiftDefaultPlugInLocations()
		var defaultManager = NSFileManager.defaultManager()
		
		for url in defaultPlugLocs {
			if let components = defaultManager.contentsOfDirectoryAtURL(url, includingPropertiesForKeys: [], options: nil, error: nil) as [NSURL]? {
				for component in components {
					if let ext = component.pathExtension {
						if ext.compare("plugin", options: .CaseInsensitiveSearch) != .OrderedSame {
							continue
						}
						if let theBundle = NSBundle(URL: component) {
						
							if let aPlug = PPSamplePlugObject(bundle: theBundle) {
								plugIns.append(aPlug)
							}
						}
					}
				}
			}
		}
		
		super.init()
	}
	
	var count: Int {
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
	
	func beginImportingSample(#type: OSType, URL url: NSURL, driver: PPDriver, parentDocument document: PPDocument, handler: (errorCode: MADErr, createdIns: PPSampleObject?) -> Void) {
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
