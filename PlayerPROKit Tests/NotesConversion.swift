//
//  NotesConversion.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/10/14.
//
//

import Foundation
import XCTest
import PlayerPROKit

class NotesConversion: XCTestCase {
	
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

	func getNotesWithLetters(useLetters useLetters: Bool) -> [String] {
		var noteNames = [String]()
		for i in 0..<96 {
			if let aNote = octaveNameFromNote(Int16(i), letters: useLetters) {
				noteNames.append(aNote)
			}
		}
		
		XCTAssert(noteNames.count == 96, "Didn't get all 96 note strings")
		
		return noteNames
	}
	
	func testNotesWithLettersTime() {
		measureBlock() {
			_ = self.getNotesWithLetters(useLetters: false)
		}
	}
	
	func testNoteConversionLetter() {
		let noteNames = getNotesWithLetters(useLetters: true)
		
		measureBlock() {
			var noteNums = [Int16]()
			for aStr in noteNames {
				if let aNoteNum: Int16 = noteFromString(aStr) {
					noteNums.append(aNoteNum)
				}
			}
			
			XCTAssert(noteNames.count == 96, "Didn't get all 96 note values converted back")
		}
	}
	
	func testNoteConversionDoReMi() {
		let noteNames = getNotesWithLetters(useLetters: false)
		
		measureBlock() {
			var noteNums = [Int16]()
			for aStr in noteNames {
				if let aNoteNum: Int16 = noteFromString(aStr) {
					noteNums.append(aNoteNum)
				}
			}
			
			XCTAssert(noteNames.count == 96, "Didn't get all 96 note values converted back")
		}
	}
	
	func testEdgeCaseStrings() {
		let edgeStrings = ["d♯4", "C-1", "re-2", "mi♯5", "g5", "so1", "ti2", "tI3", "FA♯2", "rÉ 1", "rÉ#1", "RÉ♯1"]
		for aStr in edgeStrings {
			if let _: Int16 = noteFromString(aStr) {
				
			} else {
				XCTFail("Could not decode \(aStr)")
			}
		}
	}
	
	func testInvalidNotesFromString() {
		let invalidStrs = ["", "  ", "c", "C", "---", "B75", "adda5", "♯2", "b9"]
		for aStr in invalidStrs {
			if let aNote: Int16 = noteFromString(aStr) {
				XCTFail("Accidentally got a note back, \(aStr) got converted to \(aNote)")
			}
		}
	}
	
	func testInvalidStringsFromNotes() {
		let invalidNums: [Int16] = [-1, 96, 100, 0xFF, 0xFE]
		for i in invalidNums {
			if let aStr = octaveNameFromNote(i) {
				XCTFail("Accidentally got a number back, \(i) got converted to \(aStr)")
			}
		}
	}
}
