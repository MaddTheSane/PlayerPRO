//
//  DepthController.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

import Cocoa
import PlayerPROKit

class DepthController: NSWindowController {
	dynamic var depthAmmount: Int32 = 0
	var selectionRange = NSRange(location: 0, length: 1)
	var stereoMode = false
	var currentBlock: PPPlugErrorBlock!
	var theData: PPSampleObject!
	
	var parentWindow: NSWindow!
	
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
    }

	@IBAction func okay(_ sender: AnyObject!) {
		let ourData = NSMutableData(data: theData.data)
		let rawData = UnsafeMutableRawPointer(ourData.mutableBytes)
		var Sample8Ptr = rawData.assumingMemoryBound(to: Int8.self)
		var Sample16Ptr = rawData.assumingMemoryBound(to: Int16.self)
		var Inc = depthAmmount
		if Inc == 0 {
			Inc = 1
		}
		
		switch theData.amplitude {
		case 8:
			Sample8Ptr = Sample8Ptr.advanced(by: selectionRange.location)
			var i = 0
			while i < selectionRange.length {
				var temp = Int32(Sample8Ptr.pointee)
				
				temp *= Inc;
				temp /= Int32(UInt8.max)
				
				temp *= Int32(UInt8.max)
				temp /= Inc;
				
				Sample8Ptr.pointee = Int8(temp)
				
				if stereoMode {
					Sample8Ptr = Sample8Ptr.successor()
					i += 1
				}
				
				Sample8Ptr = Sample8Ptr.successor()
				i += 1
			}
		case 16:
			Sample16Ptr = Sample16Ptr.advanced(by: selectionRange.location)
			var i = 0
			while i < selectionRange.length / 2 {	// Div 2, because it's in bytes !!!
				var temp = Int32(Sample16Ptr.pointee)
				
				temp *= Inc;
				temp /= Int32(UInt16.max)
				
				temp *= Int32(UInt16.max)
				temp /= Inc
				
				Sample16Ptr.pointee = Int16(temp)
				
				if stereoMode {
					Sample16Ptr = Sample16Ptr.successor()
					i += 1
				}
				
				Sample16Ptr = Sample16Ptr.successor()
				i += 1
			}
			
		default:
			NSBeep()
			parentWindow.endSheet(window!)
			currentBlock(.parametersErr)
			return
			
		}

		theData.data = ourData as Data
		parentWindow.endSheet(window!)
		currentBlock(.noErr);
	}
	
	@IBAction func cancel(_ sender: AnyObject!) {
		parentWindow.endSheet(window!)
		currentBlock(.userCanceledErr);
	}

}
