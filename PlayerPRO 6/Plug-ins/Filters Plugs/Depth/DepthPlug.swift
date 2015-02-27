//
//  DepthPlug.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

import Foundation
import AppKit.NSWindow
import PlayerPROKit

@objc(PPDepthPlug) final public class DepthPlug: NSObject, PPFilterPlugin {
	public let hasUIConfiguration = true;
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	override public init() {
		super.init()
	}
	
	public func runWithData(theData: PPSampleObject!, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver!) -> MADErr {
		return .OrderNotImplemented
	}
	
	public func beginRunWithData(theData: PPSampleObject!, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver!, parentWindow document: NSWindow!, handler handle: PPPlugErrorBlock!) {
		var controller = DepthController(windowNibName: "DepthController")
		controller.depthAmmount = 8
		controller.theData = theData
		controller.selectionRange = selRange
		controller.stereoMode = StereoMode
		controller.parentWindow = document

		document.beginSheet(controller.window!, completionHandler: { (returnCode) -> Void in
			
		})
	}
}
