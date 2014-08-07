//
//  PPHilightableButton.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

import Cocoa

class PPHilightableButton: NSButton {

    init(frame: NSRect) {
        super.init(frame: frame)
        // Initialization code here.
    }
	
	func setState(value: Int) {
		super.state = value
		
		self.needsDisplay = true;
	}

    override func drawRect(dirtyRect: NSRect) {
        super.drawRect(dirtyRect)
		if (self.state == NSOnState) {
			var grayOverlay = NSColor(deviceWhite:0.2, alpha:0.5);
			grayOverlay.set()
			//[grayOverlay drawSwatchInRect:dirtyRect];
			var bezierPath = NSBezierPath(ovalInRect:dirtyRect)
			bezierPath.fill()
		}

        // Drawing code here.
    }
    
}
