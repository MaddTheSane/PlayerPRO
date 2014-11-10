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
	
	func getNotesWithLetters(#useLetters: Bool) -> [String] {
		var noteNames = [String]()
		for i in 0..<96 {
			if let aNote = OctaveNameFromNote(Int16(i), letters: useLetters) {
				noteNames.append(aNote)
			}
		}
		
		XCTAssert(noteNames.count == 96, "Didn't get all 96 note strings")

		return noteNames
	}
	
	func testNotesWithLettersTime() {
		self.measureBlock {
			var noteNames = self.getNotesWithLetters(useLetters: false)
		}
	}
	
	func testNoteConversionLetter() {
		var noteNames = getNotesWithLetters(useLetters: true)
		
		self.measureBlock { () -> Void in
			var noteNums = [Int16]()
			for aStr in noteNames {
				if let aNoteNum: Int16 = NoteFromString(aStr) {
					noteNums.append(aNoteNum)
				}
			}
			
			XCTAssert(noteNames.count == 96, "Didn't get all 96 note values converted back")
		}
	}
	
	func testNoteConversionDoReMi() {
		var noteNames = getNotesWithLetters(useLetters: false)
		
		self.measureBlock { () -> Void in
			var noteNums = [Int16]()
			for aStr in noteNames {
				if let aNoteNum: Int16 = NoteFromString(aStr) {
					noteNums.append(aNoteNum)
				}
			}
			
			XCTAssert(noteNames.count == 96, "Didn't get all 96 note values converted back")
		}
	}
	
	func testEdgeCaseStrings() {
		let edgeStrings = ["d♯4", "re-2", "mi♯5", "g5", "so1", "ti2"]
		for aStr in edgeStrings {
			if let aNote: Int16 = NoteFromString(aStr) {
				
			} else {
				XCTFail("Could not decode \(aStr)")
			}
		}
	}
	
	func testInvalidNotesFromString() {
		let invalidStrs = ["", "  ", "c", "C", "---", "B75", "adda5", "♯2"]
		for aStr in invalidStrs {
			if let aNote: Int16 = NoteFromString(aStr) {
				XCTFail("Accidentally got a note back, \(aStr) got converted to \(aNote)")
			}
		}
	}
	
	func testInvalidStringsFromNotes() {
		let invalidNums: [Int16] = [-1, 96, 100, 0xFF]
		for i in invalidNums {
			if let aStr = OctaveNameFromNote(i) {
				XCTFail("Accidentally got a number back, \(i) got converted to \(aStr)")
			}
		}
	}
}
