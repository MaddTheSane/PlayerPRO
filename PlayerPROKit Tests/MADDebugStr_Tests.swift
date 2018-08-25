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

private func globalDebugFunction(line: Int16, file: UnsafePointer<Int8>?, text: UnsafePointer<Int8>?) {
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
	
	private func classDebugMethod(line: Int16, file: UnsafePointer<Int8>?, text: UnsafePointer<Int8>?) {
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
		MADRegisterDebugFunc(globalDebugFunction)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
	
	func testGlobalDebugBlock() {
		MADRegisterDebugBlock(globalDebugFunction)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
		PPLibrary.registerDebugBlock(globalDebugFunction)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
	
	func testClassDebugBlock() {
		MADRegisterDebugBlock(classDebugMethod)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
		exampleVar = "Variable 1"
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
		exampleVar = "Variable 2"
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
	
	func testPPKClassDebugBlock() {
		PPLibrary.registerDebugBlock(classDebugMethod)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
		exampleVar = "Variable 1"
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
		exampleVar = "Variable 2"
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}

	func testPPKClassCDebugFunc() {
		PPLibrary.registerDebugFunction(cDebugFunc)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
	
	func testPPKClassSwiftCDebugFunc() {
		PPLibrary.registerDebugFunction(globalDebugFunction)
		MADDebug(string: "Swift Test")
		MADDebugStr(#line, #file, "C-Style test")
	}
}
