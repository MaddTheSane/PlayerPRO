//
//  SamplePlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 2/10/15.
//
//

import Cocoa
import PlayerPROKit

final class SamplePlugHandler: NSObject, Collection, NSFastEnumeration {
	private(set) var plugIns = [PPSamplePlugObject]()
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations()
		let defaultManager = FileManager.default
		
		for url in defaultPlugLocs {
			do {
				let components = try defaultManager.contentsOfDirectory(at:url, includingPropertiesForKeys: [], options: [])
				let aComp = components.filter({ (aURL) -> Bool in
					if let ext = aURL.pathExtension {
						if ext.compare("plugin", options: .caseInsensitive) == .orderedSame {
							return true
						}
					}
					
					return false
				})
				for component in aComp {
					if let theBundle = Bundle(url: component), aPlug = PPSamplePlugObject(bundle: theBundle) {
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
	
	@nonobjc func index(after i: SamplePlugHandler.Index) -> SamplePlugHandler.Index {
		return i + 1
	}
	
	func makeIterator() -> IndexingIterator<[PPSamplePlugObject]> {
		return plugIns.makeIterator()
	}
	
	@objc subscript (index: Int) -> PPSamplePlugObject {
		return plugIns[index]
	}
	
	func beginExportingSample(_ sample: PPSampleObject, type: OSType, URL url: URL, driver: PPDriver, parentDocument document: PPDocument, handler: PPPlugErrorBlock) {
		var aPlug: PPSamplePlugObject? = nil
		
		for plug in plugIns {
			if plug.type == type {
				aPlug = plug
				break
			}
		}
		
		if let plug = aPlug {
			plug.beginExportSample(sample, to: url, driver: driver, parentDocument: document, handler: handler)
		} else {
			handler(.cannotFindPlug)
		}
	}
	
	func beginImportingSample(type: OSType, URL url: URL, driver: PPDriver, parentDocument document: PPDocument, handler: (errorCode: MADErr, createdIns: PPSampleObject?) -> Void) {
		var aPlug: PPSamplePlugObject? = nil
		
		for plug in plugIns {
			if plug.type == type {
				aPlug = plug
				break
			}
		}
		
		if let plug = aPlug {
			plug.beginImportSample(at: url, driver: driver, parentDocument: document, handler: handler)
		} else {
			handler(errorCode: .cannotFindPlug, createdIns: nil)
		}
	}
	
	func testSampleFile(_ toTest: URL, type theType: OSType) -> MADErr {
		for plug in plugIns {
			if plug.type == theType {
				return plug.canImportFile(at: toTest) ? .noErr : .fileNotSupportedByThisPlug
			}
		}

		return .cannotFindPlug
	}
	
	func isPlugAvailable(_ kind: OSType) -> Bool {
		for plug in plugIns {
			if plug.type == kind {
				return true
			}
		}
		return false
	}
	
	func identifySampleFile(_ ref: URL) -> (error: MADErr, type: OSType) {
		for plug in plugIns {
			if plug.canImportFile(at: ref) {
				return (.noErr, plug.type)
			}
		}
		
		return (.cannotFindPlug, "!!!!")
	}
	
	func countByEnumerating(with state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>!, count len: Int) -> Int {
		return (plugIns as NSArray).countByEnumerating(with: state, objects: buffer, count: len)
	}
}
