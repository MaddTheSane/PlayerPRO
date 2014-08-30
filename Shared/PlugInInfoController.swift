//
//  PPPlugInInfoController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/11/14.
//
//

import Cocoa

class PlugInInfoController: NSWindowController {
	@IBOutlet weak var nameField:			NSTextField!
	@IBOutlet weak var authorField:			NSTextField!
	@IBOutlet weak var typeField:			NSTextField!
	@IBOutlet weak var theCopyrightField:	NSTextField!
	@IBOutlet weak var pluginImage:			NSImageView!
	private var info:						PlugInInfo!

	class func windowControllerFromInfo(plugInfo: PlugInInfo) -> Self {
		var toRet = self(windowNibName:"PPPlugInInfoController")
		toRet.info = plugInfo
		
		return toRet
	}
	
    override func windowDidLoad() {
		let plugImSize = NSSize(width: 64, height: 64)
		super.windowDidLoad()
		
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		
		nameField.stringValue = self.info.plugName
		authorField.stringValue = self.info.authorName
		typeField.stringValue = self.info.plugType
		theCopyrightField.stringValue = self.info.plugCopyright
		let plugImage = NSWorkspace.sharedWorkspace().iconForFile(info.plugURL.path)
		plugImage.size = plugImSize
		pluginImage.image = plugImage
    }
	
	@IBAction func closeWindow(sender: AnyObject?) {
		NSApplication.sharedApplication().stopModal()
		self.window.close()
	}
}
