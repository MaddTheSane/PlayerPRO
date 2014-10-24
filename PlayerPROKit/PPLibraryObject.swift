//
//  PPLibraryObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 10/24/14.
//
//

import Foundation
import PlayerPROCore

private func GetArrayFromMirror<X>(mirror: MirrorType) -> [X] {
	var anArray = [X]()
	for i in 0..<mirror.count {
		var aChar = mirror[i].1.value as X
		anArray.append(aChar)
	}
	
	return anArray
}

public class PPLibraryObject: NSObject, Printable {
	public let menuName: String
	public let authorString: String
	public let plugFile: NSBundle
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
	
	override public var description: String {
		return "Name: \(menuName); Author: \(authorString); plug-in file: \(plugFile), type: \(plugType), version: \(plugVersion)"
	}
	
	internal init(plugInfo pi: UnsafePointer<PlugInfo>) {
		let unwrapped = pi.memory
		menuName = unwrapped.MenuName.takeUnretainedValue()
		authorString = unwrapped.AuthorString.takeUnretainedValue()
		plugFile = NSBundle(URL: CFBundleCopyBundleURL(unwrapped.file.takeUnretainedValue()))!
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
