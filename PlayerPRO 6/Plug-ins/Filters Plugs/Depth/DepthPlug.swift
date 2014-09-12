//
//  DepthPlug.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

import Foundation
import PlayerPROKit

@objc(PPDepthPlug) final public class DepthPlug: NSObject, PPFilterPlugin {
	public let hasUIConfiguration = true;
	
	public func runWithData(theData: PPSampleObject!, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver!) -> MADErr {
		return .OrderNotImplemented
	}
	
	public func beginRunWithData(theData: PPSampleObject!, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver!, parentDocument document: NSDocument!, handler handle: PPPlugErrorBlock!) {
		var controller = DepthController(windowNibName: "DepthController")
		controller.depthAmmount = 8
		controller.theData = theData
		controller.selectionRange = selRange
		controller.stereoMode = StereoMode

		controller.window.beginSheet(document.windowForSheet, completionHandler: { (returnCode) -> Void in
			
		})
	}
}
