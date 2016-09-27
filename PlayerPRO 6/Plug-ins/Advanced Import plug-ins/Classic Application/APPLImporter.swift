//
//  APPLImporter.swift
//  PPMacho
//
//  Created by C.W. Betts on 5/18/15.
//
//

import Cocoa
import PlayerPROKit
import SwiftAdditions
import ResourceFork

final public class APPLImporter: NSObject, PPComplexImportPlugInterface {
	//let madTypes: [OSType] = ["MADK", "MADI", "MADF", "MADG", "MADH"]
	override init() {
		super.init()
	}
	
	public convenience init?(forPlugIn: ()) {
		self.init()
	}
	
	public func beginImport(of theURL: URL, withHandler handler: @escaping PPComplexImportHandler) {
		do {
			let resFile = try FVResourceFile.resourceFileWithContentsOfURL(theURL)
			var aRet = [String: [FVResource]]()
			for resourceType in resFile.types {
				switch resourceType.type {
				case "MADK", "MADI", "MADH", "MADF", "MADG":
					aRet[resourceType.typeString] = resourceType.resources
					
				default:
					continue
				}
			}
			
			let controller = ImportWindowController(windowNibName: "PPAPPLImporter")
			controller.currentBlock = handler
			controller.resourceFile = resFile
			controller.addResourceDictionary(aRet)
			controller.beginImportModalSession()
		} catch _ {
			handler(nil, .readingErr)
		}
	}
	
	public func canImport(_ theURL: URL) throws {
		let resFile = try FVResourceFile.resourceFileWithContentsOfURL(theURL)
		for resType in resFile.types {
			switch resType.type {
			case "MADK", "MADI", "MADH", "MADF", "MADG":
				return
				
			default:
				continue
			}
		}
		
		throw MADErr.fileNotSupportedByThisPlug
	}
}
