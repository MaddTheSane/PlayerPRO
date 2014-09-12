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
	
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
    }

	@IBAction func okay(sender: AnyObject!) {
		let ourData = NSMutableData(data: theData.data)
		//let rawData = ourData.mutableBytes
		var Sample8Ptr = UnsafeMutablePointer<Int8>(ourData.mutableBytes)
		var Sample16Ptr = UnsafeMutablePointer<Int16>(ourData.mutableBytes)
		var Inc = depthAmmount
		if Inc == 0 {
			Inc = 1
		}
		
		switch theData.amplitude {
		case 8:
			Sample8Ptr = Sample8Ptr.advancedBy(selectionRange.location)
			for (var i = 0; i < selectionRange.length; i++) {
				var temp = Int32(Sample8Ptr.memory)
				
				temp *= Inc;
				temp /= Int32(UInt8.max)
				
				temp *= Int32(UInt8.max)
				temp /= Inc;
				
				Sample8Ptr.memory = Int8(temp)
				
				if stereoMode {
					Sample8Ptr++
					i++
				}
				
				Sample8Ptr++

			}
		case 16:
			Sample16Ptr = Sample16Ptr.advancedBy(selectionRange.location)
			
			for (var i = 0; i < selectionRange.length / 2; i++) {	// Div 2, because it's in bytes !!!
				var temp = Int32(Sample16Ptr.memory)
				
				temp *= Inc;
				temp /= Int32(UInt16.max)
				
				temp *= Int32(UInt16.max)
				temp /= Inc
				
				Sample16Ptr.memory = Int16(temp)
				
				if stereoMode {
					Sample16Ptr++
					i++
				}
				
				Sample16Ptr++
			}
			
		default:
			NSBeep()
			(NSApp as NSApplication).endSheet(self.window)
			currentBlock(.ParametersErr)
			return
			
		}

		theData.data = NSData(data: ourData)
		(NSApp as NSApplication).endSheet(self.window)
		currentBlock(.NoErr);
	}
	
	@IBAction func cancel(sender: AnyObject!) {
		(NSApp as NSApplication).endSheet(self.window)
		currentBlock(.UserCanceledErr);
	}

}
