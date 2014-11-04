//
//  Length.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/4/14.
//
//

import Cocoa
import PlayerPROKit.PPPlugIns

@objc(PPLengthPlug) public final class Length: NSObject, PPFilterPlugin {
	public let hasUIConfiguration = true
	
	override init() {
		super.init()
	}
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	public func runWithData(theData: PPSampleObject!, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver!) -> MADErr {
		return .OrderNotImplemented
	}
	
	public func beginRunWithData(theData: PPSampleObject!, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver!, parentWindow document: NSWindow!, handler handle: PPPlugErrorBlock!) {
		
	}
}
