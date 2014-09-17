//
//  CompFadePlug.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/15/14.
//
//

import Foundation
import PlayerPROKit.PPPlugIns

@objc(PPCompFadePlug) public final class ComplexFadePlug: NSObject, PPDigitalPlugin {
	public let hasUIConfiguration = true
	
	public func runWithPcmd(aPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver!) -> MADErr {
		return .OrderNotImplemented
	}
	
	public func beginRunWithPcmd(aPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver!, parentWindow window: NSWindow!, handler: PPPlugErrorBlock!) {
		let controller = ComplexFadeController(windowNibName: "ComplexFadeController")
		controller.thePcmd = aPcmd;
		controller.fadeType = .Instrument;
		controller.currentBlock = handler

		window.beginSheet(controller.window, completionHandler: { (retVal) -> Void in
			
		})
	}
}
