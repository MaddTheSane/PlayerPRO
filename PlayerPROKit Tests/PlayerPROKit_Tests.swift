//
//  PlayerPROKit_Tests.swift
//  PlayerPROKit Tests
//
//  Created by C.W. Betts on 9/1/14.
//
//

import Foundation
import XCTest
@testable import PlayerPROCore
@testable import PlayerPROKit
import CoreAudio
#if os(iOS)
	import CoreImage
	import UIKit
#endif

class PlayerPROKit_Tests: XCTestCase {
	var ourBundle: Bundle? = nil
	var ourLib: PPLibrary? = nil
	
	override func setUp() {
		super.setUp()
		// Put setup code here. This method is called before the invocation of each test method in the class.
		currentTestClass = self
		MADRegisterDebugFunc(cXTCFail)
		
		ourBundle = Bundle(for: PlayerPROKit_Tests.self)
			if let ourPlugPath = ourBundle?.builtInPlugInsURL, ((try? ourPlugPath.checkResourceIsReachable()) ?? false) {
				ourLib = try! PPLibrary(plugInURL: ourPlugPath)
			} else {
				ourLib = try! PPLibrary()
			}
	}
	
	override func tearDown() {
		ourLib = nil
		// Put teardown code here. This method is called after the invocation of each test method in the class.
		super.tearDown()
	}
	
	func testMADKInformation() {
		let musicPath = ourBundle!.url(forResource: "TestMADK", withExtension: "madk")!
		do {
			let info = try ourLib!.information(from: musicPath, type: "MADK")
			print(info)
			XCTAssertEqual("MADK", info.signature)
			XCTAssertEqual(info.totalPatterns, 7)
			XCTAssertEqual(info.partitionLength, 14)
			XCTAssertEqual(info.fileSize, 83671)
			XCTAssertEqual(info.totalTracks, 6)
			XCTAssertEqual(info.internalFileName, "Go For It")
			XCTAssertEqual(info.formatDescription, "MADK")
		} catch let err as NSError {
			XCTFail("Failed to get info, \(err)")
		}
	}
	
	func testS3MInformation() {
		let musicPath = ourBundle!.url(forResource: "TestS3M", withExtension: "s3m")!
		do {
			let info = try ourLib!.information(from: musicPath, type: "S3M ")
			print(info)
			XCTAssertEqual("S3M ", info.signature)
			XCTAssertEqual(info.totalPatterns, 19)
			XCTAssertEqual(info.partitionLength, 22)
			XCTAssertEqual(info.fileSize, 14189)
			//XCTAssertEqual(info.totalTracks, 0)
			XCTAssertEqual(info.internalFileName, "\"A.M. A.L. Funk\" by E-Keet")
			XCTAssertEqual(info.formatDescription, "S3M Plug")
		} catch let err as NSError {
			XCTFail("Failed to get info, \(err)")
		}
	}

	func testInvalidMADKInfo() {
		let musicPath = ourBundle!.url(forResource: "TestS3M", withExtension: "s3m")!
		do {
			_ = try ourLib!.information(from: musicPath, type: "MADK")
			XCTFail("Unexpected success")
		} catch PPMADError.fileNotSupportedByThisPlug {
			XCTAssert(true)
		} catch let error as NSError {
			XCTFail("unknown error, \(error)")
		}
	}
	
	func testObjcMADKInformation() {
		var dict: NSDictionary? = [:]
		let musicPath = ourBundle!.url(forResource: "TestMADK", withExtension: "madk")!
		var err = ourLib!.getInformation(from: musicPath, type: "MADK", info: &dict)
		XCTAssertEqual(err, MADErr.noErr)
		err = ourLib!.getInformation(from: musicPath, type: "S3M ", info: &dict)
		XCTAssertEqual(err, MADErr.fileNotSupportedByThisPlug)
	}
	
	func testLoadingUnloadingMusicFromDriver() {
		var drivSettings = MADDriverSettings.new()
		//drivSettings.driverMode = .NoHardwareDriver
		do {
			let driver = try PPDriver(library: ourLib!, settings: &drivSettings)
			let musicPath = ourBundle!.url(forResource: "TestMADK", withExtension: "madk")!
			var mus: PPMusicObject? = try PPMusicObject(url: musicPath)
			driver.currentMusic = mus
			driver.play()
			sleep(10)
			driver.stop()
			mus = nil
			driver.currentMusic = nil
			mus = try PPMusicObject(url: musicPath)
			driver.currentMusic = mus
			mus = nil
			driver.play()
			sleep(2)
			driver.currentMusic = nil
			usleep(100)
			driver.stop()
		} catch let iErr as NSError {
			XCTFail("error thrown: \(iErr)")
		}
		
	}
}
