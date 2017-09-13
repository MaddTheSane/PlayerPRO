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

final class DigitalPlugInObject : PPPlugInObject {
	private let hasUI: Bool
	private var plugCode: PPDigitalPlugin!
	override init?(bundle toInit: Bundle) {
		guard let archs = toInit.executableArchitectures else {
			return nil
		}
		
		var hasArch = false
		for arch in archs {
			if arch.intValue == NSBundleExecutableArchitectureX86_64 {
				hasArch = true
				break
			}
		}
		
		guard hasArch else {
			return nil
		}
		
		guard let rawBundClass: AnyClass = toInit.principalClass,
			let bundClass = rawBundClass as? PPDigitalPlugin.Type,
			let aPlugCode = bundClass.init(forPlugIn: ()) else {
				return nil
		}
		
		plugCode = aPlugCode
		
		hasUI = plugCode.hasUIConfiguration
		super.init(bundle: toInit)
	}
	
	override var hashValue: Int {
		return menuName.hashValue
	}
	
	override var hash: Int {
		return self.hashValue
	}

	override func isEqual(_ object: Any?) -> Bool {
		if let other = object as? DigitalPlugInObject {
			return self == other
		}
		
		return false
	}
	
	func beginCall(with myPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver, parentDocument theDoc: PPDocument, handler: @escaping PPPlugErrorBlock) {
		do {
			try plugCode.run(with: myPcmd, driver: driver)
			handler(nil)
		} catch {
			if isOrderNotImplemented(error) {
				guard let UIFunc = plugCode.beginRun else {
					handler(error)
					return
				}
				UIFunc(myPcmd, driver, theDoc.windowForSheet!, handler)
			}
			handler(error)
		}
	}
}
