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
	public let hasUIConfiguration = false
	
	@objc public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	override public init() {
		super.init()
	}
	
	public func run(withData theData: PPSampleObject, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver) -> MADErr {
		guard var ourData = theData.data, let selSwiftRange = selRange.toRange() else {
			return .parametersErr
		}
		ourData.replaceBytes(in: selSwiftRange, with: Data())
		theData.data = ourData
		return .noErr
	}
}
