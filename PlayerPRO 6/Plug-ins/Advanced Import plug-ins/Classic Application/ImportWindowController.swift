//
//  ImportWindowController.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/29/14.
//
//

import Cocoa
import PlayerPROKit

class ImportWindowController: NSWindowController {
	@IBOutlet weak var resourceNamesTable: NSTableView? = nil
	@IBOutlet weak var resourceIDsTable: NSTableView? = nil
	@IBOutlet weak var dictionaryCont: NSDictionaryController!
	@IBOutlet weak var arrayCont: NSArrayController!
	
	var currentBlock: PPComplexImportHandler!
	var resourceReference: ResFileRefNum = 0
	var resourceArray = [APPLObject]()
	var resourceDictionary = [String: [APPLObject]]()
	private var modalSession: NSModalSession!
	
	@IBAction func importMusicObject(sender: AnyObject?) {
		//TODO: implement
		
		var madMusic = UnsafeMutablePointer<MADMusic>.alloc(1)
		let ppMusic = PPMusicObject(musicStruct: madMusic, copy: false)
		
		NSApplication.sharedApplication().endModalSession(modalSession)
		currentBlock(ppMusic, .NoErr)
	}
	
	@IBAction func cancelImport(sender: AnyObject?) {
		NSApplication.sharedApplication().endModalSession(modalSession)
		currentBlock(nil, .UserCanceledErr)
	}

	func addResourceDictionary(theDict: NSDictionary) {
		if let aDict = theDict as? [String: [APPLObject]] {
			resourceDictionary = aDict
		}
	}
	
	func beginImportModalSession() {
		
		
		modalSession = NSApplication.sharedApplication().beginModalSessionForWindow(window!)
	}

	override func awakeFromNib() {
		super.awakeFromNib()
		
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		dictionaryCont.bind(NSContentDictionaryBinding, toObject: self, withKeyPath: "resourceDictionary", options: nil)
		dictionaryCont.addObserver(self, forKeyPath: "selectionIndexes", options: .New, context: nil)
		
		dictionaryCont.setSelectionIndex(0)
		
		resourceNamesTable?.sortDescriptors = [NSSortDescriptor(key: "key", ascending: true)]
		resourceIDsTable?.sortDescriptors = [NSSortDescriptor(key: "resourceID", ascending: true)]
	}
	
	override func observeValueForKeyPath(keyPath: String, ofObject object: AnyObject, change: [NSObject : AnyObject], context: UnsafeMutablePointer<Void>) {
		if object === dictionaryCont {
			if dictionaryCont.selectedObjects.count > 0 {
				let aSelect = dictionaryCont.selectedObjects[0] as NSObject // Actual class is a private class, _NSControllerKeyValuePair
				let aValue: AnyObject? = aSelect.value() // ...but it does respond to "value", which is public
				if let anotherVal = aValue as? [APPLObject] {
					self.resourceArray = anotherVal
				}

				arrayCont.bind(NSContentArrayBinding, toObject: self, withKeyPath: "resourceArray", options: nil)
			}
		} else {
			super.observeValueForKeyPath(keyPath, ofObject: object, change: change, context: context)
		}
	}
	
	deinit {
		dictionaryCont?.removeObserver(self, forKeyPath: "selectionIndexes")
		PPCloseResFile(resourceReference)
	}
}
