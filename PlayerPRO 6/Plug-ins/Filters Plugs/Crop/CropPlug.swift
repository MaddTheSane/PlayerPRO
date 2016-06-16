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
		let ourData = NSMutableData(data: theData.data)
		ourData.replaceBytes(in: selRange, withBytes: nil, length: 0)
		theData.data = ourData as Data!
		return .noErr
	}
}
