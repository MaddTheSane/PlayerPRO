//
//  ImportWindowController.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/29/14.
//
//

import Cocoa
import PlayerPROKit

class ImportWindowController: NSWindowController, NSTableViewDelegate {
	@IBOutlet weak var resourceNamesTable: NSTableView? = nil
	@IBOutlet weak var resourceIDsTable: NSTableView? = nil
	var currentBlock: PPComplexImportHandler!
	var resourceReference: ResFileRefNum = 0
	var resourceArray = [APPLObject]()
	var resourceDictionary = [String: [APPLObject]]()
	private var modalSession: NSModalSession!
	
	@IBAction func importMusicObject(sender: AnyObject?) {
		NSApplication.sharedApplication().endModalSession(modalSession)
		currentBlock(nil, .NoErr)
	}
	
	@IBAction func cancelImport(sender: AnyObject?) {
		NSApplication.sharedApplication().endModalSession(modalSession)
		currentBlock(nil, .UserCanceledErr)
	}

	func addResourceDictionary(theDict: NSDictionary) {
		if let aDict = theDict as? [String: [APPLObject]] {
			resourceDictionary = aDict
			for (key, array) in aDict {
				resourceArray += array
			}
		}
	}
	
	func beginImportModalSession() {
		
		
		modalSession = NSApplication.sharedApplication().beginModalSessionForWindow(window!)
	}

	override func awakeFromNib() {
		super.awakeFromNib()
		
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	}
	
	deinit {
		PPCloseResFile(resourceReference)
	}
}
