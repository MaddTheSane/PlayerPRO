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
	
	func getNotesWithLetters(useLetters: Bool = true) -> [String] {
		var noteNames = [String]()
		for i in 0..<96 {
			if let aNote = OctaveNameFromNote(Int16(i), letters: useLetters) {
				noteNames.append(aNote)
			}
		}
		
		return noteNames
	}
	
	func testNoteConversionLetter() {
		var noteNames = getNotesWithLetters(useLetters: true)
		XCTAssert(noteNames.count == 96, "Didn't get all 96 note strings")
		
		var noteNums = [Int16]()
		for aStr in noteNames {
			if let aNoteNum: Int16 = NoteFromString(aStr) {
				noteNums.append(aNoteNum)
			}
		}
		
		XCTAssert(noteNames.count == 96, "Didn't get all 96 note values converted back")
	}
	
	func testNoteConversionDoReMi() {
		var noteNames = getNotesWithLetters(useLetters: false)
		XCTAssert(noteNames.count == 96, "Didn't get all 96 note strings")
		
		var noteNums = [Int16]()
		for aStr in noteNames {
			if let aNoteNum: Int16 = NoteFromString(aStr) {
				noteNums.append(aNoteNum)
			}
		}
		
		XCTAssert(noteNames.count == 96, "Didn't get all 96 note values converted back")
	}
}
