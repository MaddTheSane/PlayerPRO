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
			let resFile = try FVResourceFile.resourceFileWithContents(of: theURL)
			var aRet = [String: [FVResource]]()
			for resourceType in resFile.types {
				switch resourceType.type {
				case "MADK", "MADI", "MADH", "MADF", "MADG":
					aRet[resourceType.typeString] = resourceType.resources
					
				default:
					continue
				}
			}
			
			guard aRet.count > 0 else {
				handler(nil, PPMADError(.fileNotSupportedByThisPlug, userInfo: [NSURLErrorKey: theURL]))
				return
			}
			
			let controller = ImportWindowController(windowNibName: "PPAPPLImporter")
			controller.currentBlock = handler
			controller.resourceFile = resFile
			controller.addResourceDictionary(aRet)
			controller.beginImportModalSession()
		} catch {
			handler(nil, PPMADError(.reading, userInfo: [NSUnderlyingErrorKey: error, NSURLErrorKey: theURL]))
		}
	}
	
	public func canImport(_ theURL: URL) throws {
		let resFile = try FVResourceFile.resourceFileWithContents(of: theURL)
		for resType in resFile.types {
			switch resType.type {
			case "MADK", "MADI", "MADH", "MADF", "MADG":
				return
				
			default:
				continue
			}
		}
		
		throw PPMADError(.fileNotSupportedByThisPlug)
	}
}
