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
import CoreAudio
#if os(iOS)
	import CoreImage
	import UIKit
#endif

class PlayerPROKit_Tests: XCTestCase {
	var ourBundle: NSBundle? = nil
	var ourLib: PPLibrary? = nil
	
	override func setUp() {
		super.setUp()
		// Put setup code here. This method is called before the invocation of each test method in the class.
		currentTestClass = self
		MADRegisterDebugFunc(cXTCFail)
		
		ourBundle = NSBundle(forClass: PlayerPROKit_Tests.self)
		do {
			if let ourPlugPath = ourBundle?.builtInPlugInsURL {
				try ourPlugPath.checkResourceIsReachable()
				ourLib = PPLibrary(plugInURL: ourPlugPath)
			} else {
				throw NSError(domain: PPMADErrorDomain, code: 0, userInfo: nil)
			}
		} catch {
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
		do {
			let info = try ourLib!.informationFromFile(URL: musicPath, type: "MADK")
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
	
	func testObjcMADKInformation() {
		var dict: NSDictionary = [:]
		let musicPath = ourBundle!.URLForResource("TestMADK", withExtension: "madk")!
		let err = ourLib!.getInformationFromFile(URL: musicPath, type: "MADK", info: &dict)
		XCTAssertEqual(err, MADErr.NoErr)
	}
	
	func testLoadingUnloadingMusicFromDriver() {
		var drivSettings = MADDriverSettings.new()
		//drivSettings.driverMode = .NoHardwareDriver
		do {
			let driver = try PPDriver(library: ourLib!, settings: &drivSettings)
			let musicPath = ourBundle!.URLForResource("TestMADK", withExtension: "madk")!
			var mus: PPMusicObject? = try! PPMusicObject(URL: musicPath)
			driver.currentMusic = mus
			driver.play()
			sleep(10)
			driver.stop()
			mus = nil
			driver.currentMusic = nil
			mus = try! PPMusicObject(URL: musicPath)
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
