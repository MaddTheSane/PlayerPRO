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
import ResourceFork

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
	
	@IBAction func importMusicObject(_ sender: AnyObject?) {
		if let anObject = arrayCont.selectedObjects[0] as? FVResource {
			var madMusic: UnsafeMutablePointer<MADMusic>
			var madTest: (UnsafeRawPointer!) -> MADErr
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
				NSApplication.shared().endModalSession(modalSession)
				currentBlock(nil, .parametersErr)
				
				return
			}
			
			if let aData = anObject.data {
				var errVal = madTest((aData as NSData).bytes)
				
				if errVal != .noErr {
					NSApplication.shared().endModalSession(modalSession)
					currentBlock(nil, errVal)
					
					return
				}
				
				var unusedDriverSettings = MADDriverSettings.new()
				madMusic = UnsafeMutablePointer<MADMusic>.allocate(capacity: 1)
				errVal = madLoad((aData as NSData).bytes.assumingMemoryBound(to: Int8.self), aData.count, madMusic, &unusedDriverSettings)
				
				if errVal != .noErr {
					// The importers *should* have cleaned up after themselves...
					madMusic.deallocate(capacity: 1)
					NSApplication.shared().endModalSession(modalSession)
					currentBlock(nil, errVal)
					
					return
				}
				
				let ppMusic = PPMusicObject(musicStruct: madMusic, copy: false)
				
				NSApplication.shared().endModalSession(modalSession)
				currentBlock(ppMusic, .noErr)
			} else {
				NSApplication.shared().endModalSession(modalSession)
				currentBlock(nil, .readingErr)

				return
			}
		} else {
			NSApplication.shared().endModalSession(modalSession)
			currentBlock(nil, .unknownErr)
			
			return
		}
	}
	
	@IBAction func cancelImport(_ sender: AnyObject?) {
		NSApplication.shared().endModalSession(modalSession)
		currentBlock(nil, .userCanceledErr)
	}

	func addResourceDictionary(_ theDict: [String: [FVResource]]) {
		resourceDictionary = theDict
	}
	
	func beginImportModalSession() {
		
		
		modalSession = NSApplication.shared().beginModalSession(for: window!)
	}

	override func awakeFromNib() {
		super.awakeFromNib()
		
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		dictionaryCont.bind(NSContentDictionaryBinding, to: self, withKeyPath: "resourceDictionary", options: nil)
		dictionaryCont.addObserver(self, forKeyPath: "selectionIndexes", options: .new, context: nil)
		
		dictionaryCont.setSelectionIndex(0)
		
		resourceNamesTable?.sortDescriptors = [NSSortDescriptor(key: "key", ascending: true)]
		resourceIDsTable?.sortDescriptors = [NSSortDescriptor(key: "resourceID", ascending: true)]
	}
	
	override func observeValue(forKeyPath keyPath: String?, of object: Any?, change: [NSKeyValueChangeKey : Any]?, context: UnsafeMutableRawPointer?) {
		if object as? AnyObject === dictionaryCont {
			if dictionaryCont.selectedObjects.count > 0 {
				let aValue: Any?
				if #available(OSX 10.11, *) {
				    let aSelect = dictionaryCont.selectedObjects[0] as? NSDictionaryControllerKeyValuePair
					aValue = aSelect?.value
				} else {
					// Actual class is a private class, _NSControllerKeyValuePair
					// ...but it does respond to "value", which is public
					// ...but the NSObject informal protocol was hidden in OS X 10.11,
					// so we call a wrapper function.
					aValue = GetValueUsingKVO(dictionaryCont.selectedObjects[0] as? AnyObject)
				}
				if let aValue = aValue as? [FVResource] {
					self.resourceArray = aValue
				}

				arrayCont.bind(NSContentArrayBinding, to: self, withKeyPath: "resourceArray", options: nil)
			}
		} else {
			super.observeValue(forKeyPath: keyPath, of: object, change: change, context: context)
		}
	}
	
	deinit {
		dictionaryCont?.removeObserver(self, forKeyPath: "selectionIndexes")
	}
}
