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
	typealias Index = Int
	private(set) var plugIns = [PPSamplePlugObject]()
	
	override init() {
		let defaultPlugLocs = DefaultPlugInLocations()
		let defaultManager = FileManager.default
		
		for url in defaultPlugLocs {
			do {
				let components = try defaultManager.contentsOfDirectory(at:url, includingPropertiesForKeys: [], options: [])
				let aComp = components.filter({ (aURL) -> Bool in
					let ext = aURL.pathExtension
					if ext.compare("plugin", options: .caseInsensitive) == .orderedSame {
						return true
					}
					
					return false
				})
				for component in aComp {
					if let theBundle = Bundle(url: component), let aPlug = PPSamplePlugObject(bundle: theBundle) {
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
	
	subscript (index: Int) -> PPSamplePlugObject {
		return plugIns[index]
	}
	
	func beginExportingSample(_ sample: PPSampleObject, type: OSType, URL url: URL, driver: PPDriver, parentDocument document: PPDocument, handler: @escaping PPPlugErrorBlock) {
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
			handler(MADErr.cannotFindPlug)
		}
	}
	
	func beginImportingSample(type: OSType, URL url: URL, driver: PPDriver, parentDocument document: PPDocument, handler: @escaping (_ errorCode: Error?, _ createdIns: PPSampleObject?) -> Void) {
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
			handler(MADErr.cannotFindPlug, nil)
		}
	}
	
	func testSampleFile(_ toTest: URL, type theType: OSType) throws {
		for plug in plugIns {
			if plug.type == theType {
				if plug.canImportFile(at: toTest) {
					return
				} else {
					throw MADErr.fileNotSupportedByThisPlug
				}
			}
		}

		throw MADErr.cannotFindPlug
	}
	
	func isPlugAvailable(_ kind: OSType) -> Bool {
		for plug in plugIns {
			if plug.type == kind {
				return true
			}
		}
		return false
	}
	
	func identifySampleFile(_ ref: URL) throws -> OSType {
		for plug in plugIns {
			if plug.canImportFile(at: ref) {
				return plug.type
			}
		}
		
		throw MADErr.cannotFindPlug
	}
	
	func countByEnumerating(with state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (plugIns as NSArray).countByEnumerating(with: state, objects: buffer, count: len)
	}
}
