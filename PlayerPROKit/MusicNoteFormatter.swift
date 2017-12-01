//
//  MusicNoteFormatter.swift
//  PPMacho
//
//  Created by C.W. Betts on 2/25/15.
//
//

import Foundation

@IBDesignable
public final class MusicNoteFormatter: Formatter {
	@IBInspectable public var useSingleLetter: Bool {
		get {
			return !noteOptions.contains(.solfege)
		}
		set {
			if newValue {
				noteOptions.remove(.solfege)
			} else {
				noteOptions.insert(.solfege)
			}
		}
	}
	public var noteOptions: PPSampleNoteOptions = []
	
	override public func string(for obj: Any?) -> String? {
		if let objInt = obj as? Int {
			switch objInt {
			case 0xFF, 0xFE:
				return "---"
				
			case 0..<96:
				return octaveName(fromNote: Int16(objInt), options: noteOptions)
				
			default:
				return nil
				
			}
		}
		
		return nil
	}
	
	override public func getObjectValue(_ obj: AutoreleasingUnsafeMutablePointer<AnyObject?>?, for string: String, errorDescription error: AutoreleasingUnsafeMutablePointer<NSString?>?) -> Bool {
		obj?.pointee = nil
		if string == "---" {
			obj?.pointee = NSNumber(value: 0xFF)
			if let error = error {
				error.pointee = nil
			}
			return true
		}
		
		if let aNum = note(from: string) {
			obj?.pointee = NSNumber(value: Int(aNum))
			if let error = error {
				error.pointee = nil
			}
			return true
		}
		
		if let error = error {
			error.pointee = "String outside of range"
		}

		return false
	}
}
