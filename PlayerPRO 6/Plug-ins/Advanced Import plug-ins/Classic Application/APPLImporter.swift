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
	
	public convenience init?(forPlugIn: ()) {
		//if #available(OSX 10.11, *) {
			self.init()
		//} else {
		//	return nil
		//}
	}
	
	public func beginImportOfURL(theURL: NSURL, withHandler handler: PPComplexImportHandler) {
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
			handler(nil, .ReadingErr)
		}
	}
	
	public func canImportURL(theURL: NSURL) throws {
		var outErr: NSError! = NSError(domain: "Migrator", code: 0, userInfo: nil)
		do {
			let resFile = try FVResourceFile.resourceFileWithContentsOfURL(theURL)
			for resType in resFile.types {
				switch resType.type {
					
				case "MADK", "MADI", "MADH", "MADF", "MADG":
					return
					
				default:
					continue
				}
			}
			
			outErr = createErrorFromMADErrorType(.FileNotSupportedByThisPlug)!
			throw outErr
		} catch let error as NSError {
			outErr = error
		}
		
		throw outErr
	}
}
