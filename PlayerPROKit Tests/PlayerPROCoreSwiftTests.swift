//
//  PlayerPROCoreSwiftTests.swift
//  PPMacho
//
//  Created by C.W. Betts on 4/20/15.
//
//

import Foundation
import PlayerPROCore
#if os(OSX)
	import PlayerPROKit
#endif
import XCTest

class PlayerPROCoreSwiftTests: XCTestCase {
	
	override func setUp() {
		super.setUp()
		// Put setup code here. This method is called before the invocation of each test method in the class.
	}
	
	override func tearDown() {
		// Put teardown code here. This method is called after the invocation of each test method in the class.
		super.tearDown()
	}
	
	func testDriverSettingsEqual() {
		let aSetting = MADDriverSettings.new()
		var bSetting = MADDriverSettings()
		
		XCTAssertNotEqual(aSetting, bSetting)
		
		bSetting.resetToBestDriver()
		
		XCTAssertEqual(aSetting, bSetting)
	}
}
