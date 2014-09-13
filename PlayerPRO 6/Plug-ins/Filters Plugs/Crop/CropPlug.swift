//
//  CropPlug.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

import Foundation
import PlayerPROKit.PPPlugIns
import PlayerPROKit.PPSampleObject

@objc(PPCropPlug) public final class CropPlug: NSObject, PPFilterPlugin {
	public let hasUIConfiguration = false
	
	public func runWithData(theData: PPSampleObject!, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver!) -> MADErr {
		let ourData = NSMutableData(data: theData.data)
		ourData.replaceBytesInRange(selRange, withBytes: nil, length: 0)
		theData.data = NSData(data: ourData)
		return .NoErr
	}
}
