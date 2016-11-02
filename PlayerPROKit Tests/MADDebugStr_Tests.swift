//
//  Error_Tests.swift
//  PPMacho
//
//  Created by C.W. Betts on 3/15/15.
//
//

import Foundation
import XCTest
@testable import PlayerPROCore
@testable import PlayerPROKit

private func GlobalDebugStr(line: Int16, file: UnsafePointer<Int8>?, text: UnsafePointer<Int8>?) {
	let manager = FileManager.default
	let fileStr = manager.string(withFileSystemRepresentation: file!, length: Int(strlen(file!)))
	let textStr = String(validatingUTF8: text!)!

	print("\(fileStr):\(line) Global Test: \(textStr)")
}

class MADDebugStr_Tests: XCTestCase {
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
	
	private func classDebugStr(line: Int16, file: UnsafePointer<Int8>?, text: UnsafePointer<Int8>?) {
		let manager = FileManager.default
		let fileStr = manager.string(withFileSystemRepresentation: file!, length: Int(strlen(file)))
		XCTAssertEqual(fileStr, #file, "files aren't equal...")
		var textStr = String(validatingUTF8: text!)!
		textStr += ", var: \"\(exampleVar)\""
		
		print("\(fileStr):\(line) Class Test: \(textStr)")
	}
	
	func testInlineDebugBlock() {
		MADRegisterDebugBlock { (line, file, text) -> Void in
			let manager = FileManager.default
			let fileStr = manager.string(withFileSystemRepresentation: file!, length: Int(strlen(file)))
			let textStr = String(validatingUTF8: text!)!
			
			print("\(fileStr):\(line) Inline Test: \(textStr)")
		}
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
	
	func testSwiftCDebugFunc() {
		MADRegisterDebugFunc(GlobalDebugStr)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
	
	func testGlobalDebugBlock() {
		MADRegisterDebugBlock(GlobalDebugStr)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
		PPLibrary.registerDebugBlock(GlobalDebugStr)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
	
	func testClassDebugBlock() {
		MADRegisterDebugBlock(classDebugStr)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
		exampleVar = "Variable 1"
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
		exampleVar = "Variable 2"
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
	
	func testPPClassDebugBlock() {
		PPLibrary.registerDebugBlock(classDebugStr)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
		exampleVar = "Variable 1"
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
		exampleVar = "Variable 2"
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}

	func testPPClassCDebugFunc() {
		PPLibrary.registerDebugFunction(cDebugFunc)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
	
	func testPPClassSwiftCDebugFunc() {
		PPLibrary.registerDebugFunction(GlobalDebugStr)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
}
