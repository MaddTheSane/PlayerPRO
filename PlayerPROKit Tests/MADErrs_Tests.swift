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
			let anErr = __PPCreateErrorFromMADErrorType(mad)
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
			guard let anErr = __PPCreateErrorFromMADErrorType(.needMemory) else {
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
	
	func testConvertToCocoa() {
		let errs: [MADErr] = [.needMemory, .readingErr, .incompatibleFile,
							  .parametersErr,
							  .fileNotSupportedByThisPlug, 
							  .writingErr, .userCancelledErr]
		
		errs.forEach { (err) in
			if let val = err.toNSError(convertToCocoa: true) {
				XCTAssertNotEqual(val.domain, PPMADErrorDomain, "\(err), (\(err.rawValue)) resulted in a MADErr domain")
			} else {
				XCTFail("\(err), (\(err.rawValue)) resulted in a nil value!")
			}
		}
	}

	func testAttemptToConvertToCocoa() {
		let errs: [MADErr] = [.needMemory, .readingErr, .incompatibleFile, .libraryNotInitialized,
							  .parametersErr, .unknownErr, .soundManagerErr, .orderNotImplemented,
							  .fileNotSupportedByThisPlug, .cannotFindPlug, .musicHasNoDriver,
							  .driverHasNoMusic, .soundSystemUnavailable, .writingErr, .userCancelledErr]
		
		errs.forEach { (err) in
			let val = err.toNSError(convertToCocoa: true)
			XCTAssertNotNil(val, "\(err), (\(err.rawValue)) resulted in a nil value!")
		}
	}
	
	func testAllErrors() {
		do {
			throw MADErr.needMemory
		} catch {
			
		}
		
		do {
			throw MADErr.readingErr
		} catch {
			
		}
		do {
			throw MADErr.incompatibleFile
		} catch {
			
		}
		do {
			throw MADErr.libraryNotInitialized
		} catch {
			
		}
		do {
			throw MADErr.parametersErr
		} catch {
			
		}
		do {
			throw MADErr.unknownErr
		} catch {
			
		}
		do {
			throw MADErr.soundManagerErr
		} catch {
			
		}
		do {
			throw MADErr.orderNotImplemented
		} catch {
			
		}
		do {
			throw MADErr.fileNotSupportedByThisPlug
		} catch {
			
		}
		do {
			throw MADErr.cannotFindPlug
		} catch {
			
		}
		do {
			throw MADErr.musicHasNoDriver
		} catch {
			
		}
		do {
			throw MADErr.driverHasNoMusic
		} catch {
			
		}
		do {
			throw MADErr.soundSystemUnavailable
		} catch {
			
		}
		do {
			throw MADErr.writingErr
		} catch {
			
		}
		do {
			throw MADErr.userCancelledErr
		} catch {
			
		}

	}
}
