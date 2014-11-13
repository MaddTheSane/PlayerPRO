//
//  PPLibraryObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 10/24/14.
//
//

import Foundation
import PlayerPROCore
import SwiftAdditions

public final class PPLibraryObject: NSObject, Printable, DebugPrintable {
	public let menuName: String
	public let authorString: String
	public let bundle: NSBundle
	public let plugType: String
	public let UTITypes: [String]
	public let canExport: Bool
	public let canImport: Bool
	public let plugVersion: UInt32
	public var plugMode: MADFourChar {
		if (self.canExport && self.canImport) {
			return MADPlugModes.ImportExport.rawValue
		} else if (self.canExport) {
			return MADPlugModes.Export.rawValue
		} else {
			return MADPlugModes.Import.rawValue
		}
	}
	
	public var version: NumVersion {
		return NumVersion(plugVersion)
	}
	
	override public var description: String {
		return "Name: \(menuName); Author: \(authorString); plug-in file: \(bundle.bundlePath), type: \(plugType); version: \(version)"
	}
	
	override public var debugDescription: String {
		return "Name: \(menuName); Author: \(authorString); plug-in file: \(bundle), type: \(plugType); version: \(plugVersion), \(version)"
	}
	
	internal init(plugInfo pi: UnsafePointer<PlugInfo>) {
		let unwrapped = pi.memory
		menuName = unwrapped.MenuName.takeUnretainedValue()
		authorString = unwrapped.AuthorString.takeUnretainedValue()
		bundle = NSBundle(URL: CFBundleCopyBundleURL(unwrapped.file.takeUnretainedValue()))!
		UTITypes = unwrapped.UTItypes.takeUnretainedValue() as NSArray as [String]
		let tmpArray: [CChar] = GetArrayFromMirror(reflect(unwrapped.type))
		plugType = String(CString: tmpArray, encoding: NSMacOSRomanStringEncoding)!
		plugVersion = unwrapped.version
		switch (unwrapped.mode) {
		case MADPlugModes.ImportExport.rawValue:
			canImport = true
			canExport = true
			
		case MADPlugModes.Export.rawValue:
			canImport = false
			canExport = true
			
		//case MADPlugModes.Import.rawValue:
		default:
			canImport = true
			canExport = false

		}
		
		super.init()
	}
}
