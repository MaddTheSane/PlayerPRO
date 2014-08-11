//
//  PPPlugInInfoController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/11/14.
//
//

import Cocoa

class PPPlugInInfoController: NSWindowController {

	private var info: PPPlugInInfo!
	@IBOutlet var nameField: NSTextField!
	@IBOutlet var authorField: NSTextField!
	@IBOutlet var typeField: NSTextField!
	@IBOutlet var theCopyrightField: NSTextField!
	@IBOutlet var pluginImage: NSImageView!

	class func windowControllerFromInfo(plugInfo: PPPlugInInfo) -> Self {
		var toRet = self(windowNibName:"PPPlugInInfoController")
		toRet.info = plugInfo
		
		return toRet
	}
	
	//How do I even...
	/*
	class func getNibName() -> String {
		return "PPPlugInInfoController"
	}
	
	init(plugInInfo plugInfo: PPPlugInInfo) {
		info = plugInfo
		super.init(windowNibName: "PPPlugInInfoController")
	}*/
	
    override func windowDidLoad() {
		let plugImSize = NSMakeSize(64, 64)
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		
		nameField.stringValue = self.info.plugName
		authorField.stringValue = self.info.authorName
		typeField.stringValue = self.info.plugType
		theCopyrightField.stringValue = self.info.plugCopyright
		var plugImage = NSWorkspace.sharedWorkspace().iconForFile(info.plugURL.path)
		plugImage.size = plugImSize
		pluginImage.image = plugImage
    }
	
	@IBAction func closeWindow(sender: AnyObject?) {
		NSApplication.sharedApplication().stopModal()
		self.window.close()
	}
}
