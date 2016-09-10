//
//  Length.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/4/14.
//
//

import Cocoa
import PlayerPROKit.PPPlugIns

public final class LengthPlug: NSObject, PPFilterPlugin {
	public let hasUIConfiguration = true
	
	override init() {
		super.init()
	}
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	public func run(withData theData: PPSampleObject, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver) -> MADErr {
		return .orderNotImplemented
	}
	
	public func beginRun(withData theData: PPSampleObject, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver, parentWindow document: NSWindow, handler handle: @escaping PPPlugErrorBlock){
		let controller = LengthWindowController(windowNibName: "LengthWindowController")
		controller.theData = theData
		controller.selectionRange = selRange
		controller.stereoMode = StereoMode
		controller.parentWindow = document
		
		document.beginSheet(controller.window!, completionHandler: { (returnCode) -> Void in
			
		})
	}
}
