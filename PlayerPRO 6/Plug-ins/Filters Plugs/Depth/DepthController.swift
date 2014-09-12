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

	var depthAmmount: Int32 = 0
	var selectionRange = NSRange(location: 0, length: 1)
	var stereoMode = false
	var currentBlock: PPPlugErrorBlock!
	var theData: PPSampleObject!
	
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
    }

}
