//
//  LengthWindowController.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/4/14.
//
//

import Cocoa
import PlayerPROKit

class LengthWindowController: NSWindowController {
	@IBOutlet weak var currentSize: LengthViewController!
	@IBOutlet weak var newSize: LengthViewController!
	@IBOutlet weak var theCurrentBox: NSBox!
	@IBOutlet weak var theNewBox: NSBox!
	@IBOutlet weak var lengthCompensationMatrix: NSMatrix!
	var selectionRange = NSRange(location: 0, length: 0)
	var stereoMode = false
	var currentBlock: PPPlugErrorBlock!
	var theData: PPSampleObject!

	override func windowDidLoad() {
		super.windowDidLoad()

		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		self.theCurrentBox.contentView = self.currentSize.view;
		self.theNewBox.contentView = self.newSize.view;
		currentSize.isNewSize = false;
		newSize.isNewSize = true;
	}
	
	@IBAction func okay(sender: AnyObject?) {
		NSApplication.sharedApplication().endSheet(self.window!)
		currentBlock(.NoErr)
	}
	
	@IBAction func cancel(sender: AnyObject?) {
		NSApplication.sharedApplication().endSheet(self.window!)
		currentBlock(.UserCanceledErr)
	}
	
	private func convertSampleSize(src: UnsafeMutablePointer<()>, sourceSize: Int, amplitude amp: Int16, destinationSize: Int) -> UnsafeMutablePointer<()> {
		let LRVAL = 3
		var srcSize = sourceSize
		var dstSize = destinationSize
		var src16 = UnsafeMutablePointer<Int16>(src)
		var src8 = UnsafeMutablePointer<Int8>(src)
		var tempL = 0
		var tempR = 0
		var realsrcSize = srcSize
		var left = 0
		var right = 0
		var pos = 0
		var newSize = dstSize
		
		let dst = calloc(UInt(newSize), 1)
		if (dst == nil) {
			return nil
		}
		let dst16 = UnsafeMutablePointer<Int16>(dst)
		let dst8 = UnsafeMutablePointer<Int8>(dst)
		
		realsrcSize = srcSize;
		
		srcSize /= 100;
		dstSize /= 100;
		
		switch (amp) {
		case 8:
			for (var x = 0; x < newSize; x++) {
				pos			= (x * srcSize << LRVAL) / dstSize;
				right		= pos & ((1 << LRVAL)-1);
				left		= (1 << LRVAL) - right;
				
				if (stereoMode) {
					pos >>= LRVAL
					pos /= 2
					pos *= 2
					
					if (2 + pos >= realsrcSize) {
					} else {
						tempL = (left * Int(src8[pos]) + right * Int(src8[2 + pos])) >> LRVAL
					}
					
					dst8[x] = Int8(tempL)
					
					x++;
					
					if (3 + pos >= realsrcSize) {
					} else {
						tempR = (left * Int(src8[1 + pos]) + right * Int(src8[3 + pos])) >> LRVAL
					}
					
					dst8[x] = Int8(tempR)
				} else {
					pos >>= LRVAL;
					
					if (1 + pos >= realsrcSize)	{
					} else {
						tempL = (left * Int(src8[pos]) + right * Int(src8[1 + pos])) >> LRVAL
					}
					
					dst8[x] = Int8(tempL)
				}
			}
			break;
			
		case 16:
			for (var x = 0; x < newSize/2; x++) {
				pos			= (x * srcSize << LRVAL) / dstSize
				right		= pos & ((1 << LRVAL)-1)
				left		= (1 << LRVAL) - right
				
				if (stereoMode) {
					pos >>= LRVAL
					pos /= 2
					pos *= 2
					
					if (2 + pos >= realsrcSize/2) {
					} else {
						tempL = (left * Int(src16[pos]) + right * Int(src16[2 + pos])) >> LRVAL
					}
					
					dst16[x] = Int16(tempL)
					
					x++;
					
					if (3 + pos >= realsrcSize/2) {
					} else {
						tempR = (left * Int(src16[1 + pos]) + right * Int(src16[3 + pos])) >> LRVAL
					}
					
					dst16[x] = Int16(tempR)
				} else {
					pos >>= LRVAL;
					
					if (1 + pos >= realsrcSize/2) {
					} else {
						tempL = (left * Int(src16[pos]) + right * Int(src16[1 + pos])) >> LRVAL
					}
					
					dst16[x] = Int16(tempR)
				}
			}
			break;
			
		default:
			free(dst)
			return nil
		}
		
		return dst;
	}


}
