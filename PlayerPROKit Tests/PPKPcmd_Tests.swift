//
//  PPKPcmd_Tests.swift
//  PPMacho
//
//  Created by C.W. Betts on 10/28/14.
//
//

import Foundation
import XCTest
@testable import PlayerPROKit

class PPKPcmd_Tests: XCTestCase {
	
	override func setUp() {
		super.setUp()
		// Put setup code here. This method is called before the invocation of each test method in the class.
		currentTestClass = self
		MADRegisterDebugFunc(cXTCFail)
	}

	override func tearDown() {
		// Put teardown code here. This method is called after the invocation of each test method in the class.
		super.tearDown()
	}
	
	func testAddRow() {
		var ourPcmd = PPKPcmd(tracks: 5, rows: 5)
		ourPcmd.modifyCommand(atRow: 2, track: 2) { (aCmd) -> () in
			var tmpCmd = aCmd
			tmpCmd.arg = 5
			aCmd = tmpCmd
		}
		let ourCmd = ourPcmd.getCommand(atRow: 2, track: 2)
		
		ourPcmd.addRow()
		
		let otherCmd = ourPcmd.getCommand(atRow: 2, track: 2)
		
		XCTAssertEqual(ourCmd, otherCmd, "Row added unsuccessfully")
	}
	
	func testAddTrack() {
		var ourPcmd = PPKPcmd(tracks: 5, rows: 5)
		for i in 0 ..< 25 {
			ourPcmd.modifyCommand(atRow: Int16(i % 5), track: Int16(i / 5)) { (aCmd) -> () in
				var tmpCmd = aCmd
				tmpCmd.arg = MADByte(i)
				aCmd = tmpCmd
			}
		}
		let ourCmd = ourPcmd.getCommand(atRow: 2, track: 2)
		let ourCmd5 = ourPcmd.getCommand(atRow: 4, track: 4)
		
		ourPcmd.addTrack()
		
		let otherCmd = ourPcmd.getCommand(atRow: 2, track: 2)
		let otherCmd5 = ourPcmd.getCommand(atRow: 4, track: 4)
		XCTAssertEqual(ourCmd, otherCmd, "Track added unsuccessfully")
		XCTAssertEqual(ourCmd5, otherCmd5, "Track added unsuccessfully")
	}
}
