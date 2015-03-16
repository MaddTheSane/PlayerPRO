//
//  PlayerPROKit_Tests.swift
//  PlayerPROKit Tests
//
//  Created by C.W. Betts on 9/1/14.
//
//

import Foundation
import XCTest
import PlayerPROCore
import PlayerPROKit

class PlayerPROKit_Tests: XCTestCase {
	var ourBundle: NSBundle? = nil
	var ourLib: PPLibrary? = nil
	
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
		ourBundle = NSBundle(forClass: PlayerPROKit_Tests.self)
		if let ourPlugPath = ourBundle?.builtInPlugInsURL {
			ourLib = PPLibrary(plugInURL: ourPlugPath)
		} else {
			ourLib = PPLibrary()
		}
    }
	
    override func tearDown() {
		ourLib = nil
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }

	func testMADKInformation() {
		let musicPath = ourBundle!.URLForResource("TestMADK", withExtension: "madk")!
		let info = ourLib!.informationFromFile(URL: musicPath, type: "MADK")
		XCTAssert(info.error == .NoErr)
		let unwrapped = info.info!
		println(unwrapped)
		XCTAssertEqual("MADK", unwrapped.signature)
		XCTAssertEqual(unwrapped.totalPatterns, 7)
		XCTAssertEqual(unwrapped.partitionLength, 14)
		XCTAssertEqual(unwrapped.fileSize, 83671)
		XCTAssertEqual(unwrapped.totalTracks, 6)
		XCTAssertEqual(unwrapped.internalFileName, "Go For It")
		XCTAssertEqual(unwrapped.formatDescription, "MADK")
	}
	
	func testObjcMADKInformation() {
		var dict: NSDictionary = [:]
		let musicPath = ourBundle!.URLForResource("TestMADK", withExtension: "madk")!
		let err = ourLib!.getInformationFromFile(URL: musicPath, type: "MADK", info: &dict)
		XCTAssertEqual(err, MADErr.NoErr)
	}
}
