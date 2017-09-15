//
//  CropPlug.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

import Foundation
import PlayerPROKit

public final class Crop: NSObject, PPFilterPlugin {
	public var hasUIConfiguration: Bool {
		return false
	}
	
	@objc public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	override public init() {
		super.init()
	}
	
	public func run(withData theData: PPSampleObject, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver) throws {
		guard var ourData = theData.data, let selSwiftRange = selRange.toRange() else {
			throw MADErr.parametersErr
		}
		ourData.replaceSubrange(selSwiftRange, with: Data())
		theData.data = ourData
	}
}
