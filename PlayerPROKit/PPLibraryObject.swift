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

/// A plug-in that PlayerPROKit can use to import and/or export tracker files.
@objc public final class PPLibraryObject: CustomStringConvertible, CustomDebugStringConvertible {
	/// The menu name of the tracker importer. Might be localized.
	public let menuName: String
	
	/// The author of the tracker. Might be localized.
	public let authorString: String
	
	/// The bundle referencing the plug-in.
	public let bundle: NSBundle
	
	/// A tuple of the plug-in identifier.
	///
	/// Historically, this was the file type that the Classic Mac OS would use to identify the file.
	public let tupleType: (Int8, Int8, Int8, Int8, Int8)
	
	/// An `NSString` of the plug-in identifier.
	///
	/// Historically, this was the file type that the Classic Mac OS would use to identify the file.
	public let type: String
	
	/// An array of UTIs that the plug-in can open.
	public let UTITypes: [String]
	
	/// Can the plug-in export to this tracker type?
	public let canExport: Bool
	
	/// Can the plug-in import the tracker type?
	public let canImport: Bool
	
	/// The version of the plug-in
	public let plugVersion: UInt32
	
	///The plug-in mode.
	///
	///The mode indicates, via `OSType`, the export and import capabilities of the plug-in.
	///You can just use `canImport` and `canExport` instead.
	public var mode: MADFourChar {
		if (self.canExport && self.canImport) {
			return MADPlugModes.ImportExport.rawValue
		} else if (self.canExport) {
			return MADPlugModes.Export.rawValue
		} else {
			return MADPlugModes.Import.rawValue
		}
	}
	
	public var description: String {
		return "Name: \(menuName); Author: \(authorString); plug-in file: \(bundle.bundlePath), type: \(type)"
	}
	
	public var debugDescription: String {
		return "Name: \(menuName); Author: \(authorString); plug-in file: \(bundle), type: \(type); version: \(plugVersion)"
	}
	
	internal init(plugInfo unwrapped: PlugInfo) {
		menuName = unwrapped.MenuName.takeUnretainedValue() as String
		authorString = unwrapped.AuthorString.takeUnretainedValue() as String
		bundle = NSBundle(URL: CFBundleCopyBundleURL(unwrapped.file.takeUnretainedValue()))!
		UTITypes = unwrapped.UTItypes.takeUnretainedValue() as! [String]
		tupleType = unwrapped.type
		let tmpArray: [CChar] = getArrayFromMirror(reflect(tupleType))
		type = String(CString: tmpArray, encoding: NSMacOSRomanStringEncoding)!
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
	}
}
