//
//  MusicNoteFormatter.swift
//  PPMacho
//
//  Created by C.W. Betts on 2/25/15.
//
//

import Foundation

@IBDesignable
public final class MusicNoteFormatter: NSFormatter {
	@IBInspectable public var useSingleLetter = true
	
	override public func stringForObjectValue(obj: AnyObject) -> String? {
		if let objInt = obj as? Int {
			switch objInt {
			case 0xFF, 0xFE:
				return "---"
				
			case 0..<96:
				return OctaveNameFromNote(Int16(objInt), letters: useSingleLetter)
				
			default:
				return nil
				
			}
		}
		
		return nil
	}
	
	override public func getObjectValue(obj: AutoreleasingUnsafeMutablePointer<AnyObject?>, forString string: String, errorDescription error: AutoreleasingUnsafeMutablePointer<NSString?>) -> Bool {
		obj.memory = nil
		if string == "---" {
			obj.memory = 0xFF
			if error != nil {
				error.memory = nil
			}
			return true
		}
		
		if let aNum = NoteFromString(string) {
			obj.memory = Int(aNum)
			if error != nil {
				error.memory = nil
			}
			return true
		}
		
		if error != nil {
			error.memory = "String outside of range"
		}

		return false
	}
}
