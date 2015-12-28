//
//  DigitalPlugInObject.swift
//  PPMacho
//
//  Created by C.W. Betts on 5/13/15.
//
//

import CoreFoundation
import PlayerPROCore
import PlayerPROCore.PlugIns
import Foundation
import PlayerPROKit
import SwiftAdditions

func ==(lhs: DigitalPlugInObject, rhs: DigitalPlugInObject) -> Bool {
	if lhs.menuName != rhs.menuName {
		return false
	} else if lhs.file != rhs.file {
		return false
	}
	
	//Ignore version, authorstring and type
	return true
}

class DigitalPlugInObject : PPPlugInObject {
	private let hasUI: Bool
	private var plugCode: PPDigitalPlugin!
	override init?(bundle toInit: NSBundle) {
		if let archs = toInit.executableArchitectures  {
			var hasArch = false
			for arch in archs {
				if arch == NSBundleExecutableArchitectureX86_64 {
					hasArch = true
					break
				}
			}
			
			if !hasArch {
				hasUI = false
				super.init(bundle: toInit)
				return nil
			}
			
			if let rawBundClass: AnyClass = toInit.principalClass, bundClass = rawBundClass as? PPDigitalPlugin.Type {
				plugCode = bundClass.init(forPlugIn: ())
			} else {
				hasUI = false
				super.init(bundle: toInit)
				return nil
			}
			
			hasUI = plugCode.hasUIConfiguration
			super.init(bundle: toInit)
			
		} else {
			hasUI = false
			super.init(bundle: toInit)
			return nil
		}
	}
	
	override var hashValue: Int {
		return menuName.hashValue
	}
	
	override var hash: Int {
		return self.hashValue
	}

	override func isEqual(object: AnyObject?) -> Bool {
		if let other = object as? DigitalPlugInObject {
			return self == other
		}
		
		return false
	}
	
	func beginCallWithPcmd(myPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver, parentDocument theDoc: PPDocument, handler: PPPlugErrorBlock) {
		let outError = plugCode.runWithPcmd(myPcmd, driver: driver)
		if outError == .OrderNotImplemented {
			guard let aVoid = plugCode.beginRunWithPcmd else {
				handler(outError)
				return
			}
			aVoid(myPcmd, driver: driver, parentWindow: theDoc.windowForSheet!, handler: handler)
		}
		handler(outError)
	}
}
