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
#if !os(OSX)
	import MobileCoreServices
#endif

/// A plug-in that PlayerPROKit can use to import and/or export tracker files.
@objc public final class PPLibraryObject: NSObject {
	/// The menu name of the tracker importer. Might be localized.
	@objc public let menuName: String
	
	/// The author of the tracker. Might be localized.
	@objc public let authorString: String
	
	/// The bundle referencing the plug-in.
	@objc public let bundle: Bundle
	
	/// A tuple of the plug-in identifier.
	///
	/// Historically, this was the file type that the Classic Mac OS would use to identify the file.
	public let tupleType: (Int8, Int8, Int8, Int8, Int8)
	
	/// A `String` of the plug-in identifier.
	///
	/// Historically, this was the file type that the Classic Mac OS would use to identify the file.
	@objc public let type: String
	
	/// An array of UTIs that the plug-in can open.
	@objc public let UTITypes: [String]
	
	/// Can the plug-in export to this tracker type?
	@objc public let canExport: Bool
	
	/// Can the plug-in import the tracker type?
	@objc public let canImport: Bool
	
	/// The version of the plug-in
	@objc public let plugVersion: UInt32
	
	/// The file extensions associated with this plug-in.
	@objc public var fileExtensions: [String] {
		var theOpenables = Set<String>()
		for anUTI in UTITypes {
			guard let unPreArr = UTTypeCopyAllTagsWithClass(anUTI as NSString, kUTTagClassFilenameExtension)?.takeRetainedValue() as NSArray?,
				let anArr = unPreArr as? [String] else {
					continue
			}
			
			theOpenables.formIntersection(anArr)
		}
		return Array(theOpenables).sorted()
	}
	
	#if os(OSX)
	/// the File Type `OSType` associated with this plug-in.
	@objc public var fileTypeCodes: [OSType]? {
		var theOpenables = [String]()
		for anUTI in UTITypes {
			guard let unPreArr = UTTypeCopyAllTagsWithClass(anUTI as NSString, kUTTagClassOSType)?.takeRetainedValue() as NSArray?,
				let anArr = unPreArr as? [String] else {
					continue
			}
			
			theOpenables += anArr
		}
		
		var fileTypes = theOpenables.map { UTGetOSTypeFromString($0 as NSString) }
		
		fileTypes.insert(OSType(tupleType), at: 0)
		
		return fileTypes
	}
	#endif
	
	///The plug-in mode.
	///
	///The mode indicates, via `OSType`, the export and import capabilities of the plug-in.
	///You can just use `canImport` and `canExport` instead.
	public var mode: MADFourChar {
		if self.canExport && self.canImport {
			return MADPlugModes.importExport.rawValue
		} else if self.canExport {
			return MADPlugModes.export.rawValue
		} else {
			return MADPlugModes.import.rawValue
		}
	}
	
	public override var description: String {
		return "Name: '\(menuName)'; Author: \(authorString); plug-in file: \(bundle.bundlePath), type: '\(type)'"
	}
	
	public override var debugDescription: String {
		return "Name: '\(menuName)'; Author: \(authorString); plug-in file: \(bundle), type: '\(type)'; version: \(plugVersion)"
	}
	
	internal init(plugInfo unwrapped: PlugInfo) {
		menuName = unwrapped.MenuName.takeUnretainedValue() as String
		authorString = unwrapped.AuthorString.takeUnretainedValue() as String
		bundle = Bundle(url: CFBundleCopyBundleURL(unwrapped.file.takeUnretainedValue()) as URL)!
		UTITypes = unwrapped.UTItypes.takeUnretainedValue() as NSArray as! [String]
		tupleType = unwrapped.type
		let tmpArray: [CChar] = try! arrayFromObject(reflecting: tupleType)
		type = String(cString: tmpArray, encoding: String.Encoding.macOSRoman)!
		plugVersion = unwrapped.version
		switch unwrapped.mode {
		case MADPlugModes.importExport.rawValue:
			canImport = true
			canExport = true
			
		case MADPlugModes.export.rawValue:
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
