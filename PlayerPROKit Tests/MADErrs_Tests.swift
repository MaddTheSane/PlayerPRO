//
//  MADErrs.swift
//  PPMacho
//
//  Created by C.W. Betts on 12/21/15.
//
//

import XCTest
@testable import PlayerPROCore
@testable import PlayerPROKit

class MADErrs_Tests: XCTestCase {

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

    func testPPCreateError() {
		let errors: [(MADErr, Int)] = [(.needMemory, -1),
		(.readingErr, -2), (.incompatibleFile, -3),
		(.libraryNotInitialized, -4), (.parametersErr, -5)]
		for (mad, raw) in errors {
			let anErr = PPCreateErrorFromMADErrorType(mad)
			XCTAssertNotNil(anErr)
			if let anErr = anErr {
				XCTAssertEqual(anErr._code, raw)
			}
		}
    }
	
	func testSwiftMADErrErrorType() {
		do {
			throw MADErr.needMemory
		} catch let error as NSError {
			guard let anErr = PPCreateErrorFromMADErrorType(.needMemory) else {
				XCTFail("huh!?")
				return
			}
			XCTAssertEqual(error.domain, anErr._domain)
			XCTAssertEqual(error.code, anErr._code)
		}
	}
	
	func testSwiftToNSError() {
		if let anNSErr = MADErr.needMemory.toNSError(convertToCocoa: false) {
			XCTAssertEqual(anNSErr.domain, PPMADErrorDomain)
		}
		
		if let anNSErr = MADErr.needMemory.toNSError(customUserDictionary:["CustomInfo": "Hello"], convertToCocoa: false) {
			XCTAssertEqual(anNSErr.domain, PPMADErrorDomain)
		}

	}
}
