//
//  Error_Tests.swift
//  PPMacho
//
//  Created by C.W. Betts on 3/15/15.
//
//

import Foundation
import XCTest
import PlayerPROCore
import PlayerPROKit

private func GlobalDebugStr(line: Int16, file: UnsafePointer<Int8>, text: UnsafePointer<Int8>) {
	let manager = NSFileManager.defaultManager()
	let fileStr = manager.stringWithFileSystemRepresentation(file, length: Int(strlen(file)))
	let textStr = String(UTF8String: text)!

	print("\(fileStr):\(line) Global Test: \(textStr)")
}

class Error_Tests: XCTestCase {
	var exampleVar = ""
	
	override func setUp() {
		super.setUp()
		// Put setup code here. This method is called before the invocation of each test method in the class.
		currentTestClass = self
		MADRegisterDebugFunc(cXTCFail)

		//var exampleVar = ""
	}
	
	override func tearDown() {
		PPLibrary.deregisterDebugFunction()
		// Put teardown code here. This method is called after the invocation of each test method in the class.
		super.tearDown()
	}
	
	private func classDebugStr(line: Int16, file: UnsafePointer<Int8>, text: UnsafePointer<Int8>) {
		let manager = NSFileManager.defaultManager()
		let fileStr = manager.stringWithFileSystemRepresentation(file, length: Int(strlen(file)))
		XCTAssertEqual(fileStr, __FILE__, "files aren't equal...")
		var textStr = String.fromCString(text)!
		textStr += ", var: \"\(exampleVar)\""
		
		print("\(fileStr):\(line) Class Test: \(textStr)")
	}
	
	func testInlineDebugBlock() {
		MADRegisterDebugBlock { (line, file, text) -> Void in
			let manager = NSFileManager.defaultManager()
			let fileStr = manager.stringWithFileSystemRepresentation(file, length: Int(strlen(file)))
			let textStr = String(UTF8String: text)!
			
			print("\(fileStr):\(line) Inline Test: \(textStr)")
		}
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
	}
	
	func testSwiftCDebugFunc() {
		MADRegisterDebugFunc(GlobalDebugStr)
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
	}
	
	func testGlobalDebugBlock() {
		MADRegisterDebugBlock(GlobalDebugStr)
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
		PPLibrary.registerDebugBlock(GlobalDebugStr)
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
	
	func testPPClassDebugBlock() {
		PPLibrary.registerDebugBlock(classDebugStr)
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
		exampleVar = "Variable 1"
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
		exampleVar = "Variable 2"
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
	}

	func testPPClassCDebugFunc() {
		PPLibrary.registerDebugFunction(cDebugFunc)
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
	}
	
	func testPPClassSwiftCDebugFunc() {
		PPLibrary.registerDebugFunction(GlobalDebugStr)
		MADDebugString("Swift Test")
		MADDebugStr(__LINE__, __FILE__, "C-Style test")
	}
}
