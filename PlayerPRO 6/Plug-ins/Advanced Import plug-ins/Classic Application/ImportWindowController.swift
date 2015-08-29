//
//  ImportWindowController.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/29/14.
//
//

import Cocoa
import PlayerPROKit
import SwiftAdditions

class ImportWindowController: NSWindowController {
	@IBOutlet weak var resourceNamesTable: NSTableView? = nil
	@IBOutlet weak var resourceIDsTable: NSTableView? = nil
	@IBOutlet weak var dictionaryCont: NSDictionaryController!
	@IBOutlet weak var arrayCont: NSArrayController!
	
	var currentBlock: PPComplexImportHandler!
	var resourceFile: FVResourceFile!
	dynamic var resourceArray = [FVResource]()
	var resourceDictionary = [String: [FVResource]]()
	private var modalSession: NSModalSession!
	
	@IBAction func importMusicObject(sender: AnyObject?) {
		if let anObject = arrayCont.selectedObjects[0] as? FVResource {
			var madMusic: UnsafeMutablePointer<MADMusic>
			var madTest: (UnsafePointer<Void>) -> MADErr
			var madLoad: (UnsafePointer<Int8>, size_t, UnsafeMutablePointer<MADMusic>, UnsafeMutablePointer<MADDriverSettings>) -> MADErr
			switch anObject.type!.type {
			case "MADI":
				madTest = TESTMADI
				madLoad = MADI2Mad
				
			case "MADF", "MADG":
				madTest = TestMADFGFile
				madLoad = MADFG2Mad
				
			case "MADH":
				madTest = TESTMADH
				madLoad = MADH2Mad
				
			case "MADK":
				madTest = TESTMADK
				madLoad = LoadMADK
				
			default:
				NSApplication.sharedApplication().endModalSession(modalSession)
				currentBlock(nil, .ParametersErr)
				
				return
			}
			
			if let aData = anObject.data {
				var errVal = madTest(aData.bytes)
				
				if errVal != .NoErr {
					NSApplication.sharedApplication().endModalSession(modalSession)
					currentBlock(nil, errVal)
					
					return
				}
				
				var unusedDriverSettings = MADDriverSettings.new()
				madMusic = UnsafeMutablePointer<MADMusic>.alloc(1)
				errVal = madLoad(UnsafePointer<Int8>(aData.bytes), aData.length, madMusic, &unusedDriverSettings)
				
				if errVal != .NoErr {
					// The importers *should* have cleaned up after themselves...
					madMusic.dealloc(1)
					NSApplication.sharedApplication().endModalSession(modalSession)
					currentBlock(nil, errVal)
					
					return
				}
				
				let ppMusic = PPMusicObject(musicStruct: madMusic, copy: false)
				
				NSApplication.sharedApplication().endModalSession(modalSession)
				currentBlock(ppMusic, .NoErr)
			} else {
				NSApplication.sharedApplication().endModalSession(modalSession)
				currentBlock(nil, .ReadingErr)

				return
			}
		} else {
			NSApplication.sharedApplication().endModalSession(modalSession)
			currentBlock(nil, .UnknownErr)
			
			return
		}
	}
	
	@IBAction func cancelImport(sender: AnyObject?) {
		NSApplication.sharedApplication().endModalSession(modalSession)
		currentBlock(nil, .UserCanceledErr)
	}

	func addResourceDictionary(theDict: [String: [FVResource]]) {
		resourceDictionary = theDict
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
	
	override func observeValueForKeyPath(keyPath: String?, ofObject object: AnyObject?, change: [String : AnyObject]?, context: UnsafeMutablePointer<Void>) {
		if object === dictionaryCont {
			if dictionaryCont.selectedObjects.count > 0 {
				let aValue: AnyObject?
				if #available(OSX 10.11, *) {
				    let aSelect = dictionaryCont.selectedObjects[0] as! NSDictionaryControllerKeyValuePair
					aValue = aSelect.value // ...but it does respond to "value", which is public
				} else {
					// Actual class is a private class, _NSControllerKeyValuePair
					// ...but it does respond to "value", which is public
					// ...but the NSObject informal protocol was hidden in OS X 10.11,
					// so we call a wrapper function.
					aValue = GetValueUsingKVO(dictionaryCont.selectedObjects[0])
				}
				if let anotherVal = aValue as? [FVResource] {
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
	}
}
