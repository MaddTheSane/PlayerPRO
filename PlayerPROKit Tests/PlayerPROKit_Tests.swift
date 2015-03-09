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

func GlobalDebugStr(line: Int16, file: UnsafePointer<Int8>, text: UnsafePointer<Int8>) {
	let manager = NSFileManager.defaultManager()
	let fileStr = manager.stringWithFileSystemRepresentation(file, length: Int(strlen(file)))
	let textStr = String(UTF8String: text)!

	println("\(fileStr):\(line) Global Test: \(textStr)")
}

class PlayerPROKit_Tests: XCTestCase {
	var ourBundle: NSBundle? = nil
	var ourLib: PPLibrary? = nil
	var exampleVar = ""
	
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.
		ourBundle = NSBundle(forClass: PlayerPROKit_Tests.self)
		MADRegisterDebugBlock(nil)
		if let ourPlugPath = ourBundle?.builtInPlugInsURL {
			ourLib = PPLibrary(plugInURL: ourPlugPath)
		} else {
			ourLib = PPLibrary()
		}
		var exampleVar = ""
    }
	
    override func tearDown() {
		ourLib = nil
		MADRegisterDebugBlock(nil)
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }

	func classDebugStr(line: Int16, file: UnsafePointer<Int8>, text: UnsafePointer<Int8>) {
		let manager = NSFileManager.defaultManager()
		let fileStr = manager.stringWithFileSystemRepresentation(file, length: Int(strlen(file)))
		XCTAssertEqual(fileStr, __FILE__, "files aren't equal...")
		var textStr = String.fromCString(text)!
		textStr += ", var: \"\(exampleVar)\""
		
		println("\(fileStr):\(line) Class Test: \(textStr)")
	}
	
	func testInlineDebugBlock() {
		MADRegisterDebugBlock { (line, file, text) -> Void in
			let manager = NSFileManager.defaultManager()
			let fileStr = manager.stringWithFileSystemRepresentation(file, length: Int(strlen(file)))
			let textStr = String(UTF8String: text)!
			
			println("\(fileStr):\(line) Inline Test: \(textStr)")
		}
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
	}
	
	func testGlobalDebugBlock() {
		MADRegisterDebugBlock(GlobalDebugStr)
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
	}
	
	func testClassDebugBlock() {
		MADRegisterDebugBlock(classDebugStr)
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
		exampleVar = "Variable 1"
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
		exampleVar = "Variable 2"
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
	}
}
