//
//  PPPlugInInfoController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/11/14.
//
//

import Cocoa

final class PlugInInfoController: NSWindowController {
	@IBOutlet weak var nameField:			NSTextField!
	@IBOutlet weak var authorField:			NSTextField!
	@IBOutlet weak var typeField:			NSTextField!
	@IBOutlet weak var theCopyrightField:	NSTextField!
	@IBOutlet weak var pluginImage:			NSImageView!
	private var info:						PlugInInfo!

	class func windowController(from plugInfo: PlugInInfo) -> Self {
		let toRet = self.init(windowNibName: NSNib.Name(rawValue: "PPPlugInInfoController"))
		toRet.info = plugInfo
		
		return toRet
	}
	
    override func windowDidLoad() {
		let plugImSize = NSSize(width: 64, height: 64)
		super.windowDidLoad()
		
		nameField.stringValue = info.plugName
		authorField.stringValue = info.authorName
		typeField.stringValue = info.plugType
		theCopyrightField.stringValue = info.plugCopyright
		let plugImage = NSWorkspace.shared.icon(forFile: info.plugURL.path)
		plugImage.size = plugImSize
		pluginImage.image = plugImage
    }
	
	@IBAction func closeWindow(_ sender: AnyObject?) {
		NSApplication.shared.stopModal()
		window?.close()
	}
}
