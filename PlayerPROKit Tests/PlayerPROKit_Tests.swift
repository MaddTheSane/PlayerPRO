//
//  PlayerPROKit_Tests.swift
//  PlayerPROKit Tests
//
//  Created by C.W. Betts on 9/1/14.
//
//

import Foundation
import XCTest
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
		MADRegisterDebugBlock(nil)
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
	
	func testDebugString() {
		MADRegisterDebugBlock { (line, file, text) -> Void in
			let manager = NSFileManager.defaultManager()
			let fileStr = manager.stringWithFileSystemRepresentation(file, length: Int(strlen(file)))
			let textStr = String(UTF8String: text)!
			
			println("\(fileStr):\(line) \(textStr)")
		}
		MADDebugString("A Quick Test")
	}
	
}
