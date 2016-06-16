//
//  CompFadePlug.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/15/14.
//
//

import Foundation
import PlayerPROKit.PPPlugIns

public final class ComplexFade: NSObject, PPDigitalPlugin {
	public let hasUIConfiguration = true
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	override public init() {
		super.init()
	}
	
	public func run(with aPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver) -> MADErr {
		return .orderNotImplemented
	}
	
	public func beginRun(with aPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver, parentWindow window: NSWindow, handler: PPPlugErrorBlock) {
		let controller = ComplexFadeController(windowNibName: "ComplexFadeController")
		controller.thePcmd = aPcmd;
		controller.fadeType = .instrument;
		controller.currentBlock = handler
		controller.parentWindow = window

		window.beginSheet(controller.window!, completionHandler: { (retVal) -> Void in
			
		})
	}
}
