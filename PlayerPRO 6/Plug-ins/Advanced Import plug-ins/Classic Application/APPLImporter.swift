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

final public class APPLImporter: NSObject, PPComplexImportPlugInterface {
	//let madTypes: [OSType] = ["MADK", "MADI", "MADF", "MADG", "MADH"]
	override init() {
		super.init()
	}
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	public func beginImportOfURL(theURL: NSURL, withHandler handler: PPComplexImportHandler) {
		if let resFile = FVResourceFile.resourceFileWithContentsOfURL(theURL, error: nil) {
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
		} else {
			handler(nil, .ReadingErr)
		}
	}
	
	public func canImportURL(theURL: NSURL, error outErr: NSErrorPointer) -> Bool {
		if let resFile = FVResourceFile.resourceFileWithContentsOfURL(theURL, error: outErr) {
			for resType in resFile.types {
				switch resType.type {
					
				case "MADK", "MADI", "MADH", "MADF", "MADG":
					return true
					
				default:
					continue
				}
			}
			
			if outErr != nil {
				outErr.memory = createErrorFromMADErrorType(.FileNotSupportedByThisPlug)
			}
			return false
		}
		
		return false
	}
}
