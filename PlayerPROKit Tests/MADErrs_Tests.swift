//
//  MADErrs.swift
//  PPMacho
//
//  Created by C.W. Betts on 12/21/15.
//
//

import XCTest
import PlayerPROCore
import PlayerPROKit

class MADErrs_Tests: XCTestCase {

    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }

    func testPPCreateError() {
		let errors: [(MADErr, Int)] = [(.NeedMemory, -1)]
		for (mad, raw) in errors {
			let anErr = PPCreateErrorFromMADErrorType(mad)
			XCTAssertNotNil(anErr)
			if let anErr = anErr {
				XCTAssertEqual(anErr.code, raw)
			}
		}
    }
	
	func testSwiftMADErrErrorType() {
		do {
			throw MADErr.NeedMemory
		} catch let error as NSError {
			guard let anErr = PPCreateErrorFromMADErrorType(.NeedMemory) else {
				XCTAssert(false, "huh!?")
				return
			}
			XCTAssertEqual(error.domain, anErr.domain)
			XCTAssertEqual(error.code, anErr.code)
		}
	}
}
