//
//  InstrumentViewController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/22/14.
//
//

import Cocoa
import PlayerPROKit

class InstrumentViewController: NSViewController, NSOutlineViewDataSource, NSOutlineViewDelegate {
	@IBOutlet var infoDrawer: NSDrawer!
	@IBOutlet var instrumentSize: NSTextField!
	@IBOutlet var instrumentLoopStart: NSTextField!
	@IBOutlet var instrumentLoopSize: NSTextField!
	@IBOutlet var instrumentVolume: NSTextField!
	@IBOutlet var instrumentRate: NSTextField!
	@IBOutlet var instrumentNote: NSTextField!
	@IBOutlet var instrumentBits: NSTextField!
	@IBOutlet var instrumentMode: NSTextField!
	@IBOutlet var waveFormImage: NSImageView!
	@IBOutlet var instrumentOutline: NSOutlineView!
	
	weak var importer: PPInstrumentPlugHandler!
	weak var filterHandler: PPFilterPlugHandler!
	@IBOutlet var currentDocument: PPDocument!
	weak var theDriver: PPDriver!

	func colorsDidChange(aNot: NSNotification) {
	
	}

	func musicDidChange(aNot: NSNotification) {
		
	}

	private func loadInstrumentsFromMusic() {
		if (instrumentOutline != nil) {
			instrumentOutline.reloadData()
			instrumentOutline.selectRowIndexes(NSIndexSet(index: 0), byExtendingSelection: false)
			instrumentOutline.scrollToBeginningOfDocument(nil)
		}
	}

	
	func playSample(fromInstrument: Int16, sampleNumber: Int16, volume: UInt8 = 0xFF, note: UInt8 = 0xFF) {
		
	}

	override func awakeFromNib() {
		super.awakeFromNib()
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		instrumentOutline.selectRowIndexes(NSIndexSet(index: 0), byExtendingSelection: false)
		theDriver = currentDocument.theDriver;
	}

	
	@IBAction func exportInstrument(sender: AnyObject!) {
		
	}
	
	@IBAction func importInstrument(sender: AnyObject!) {
		
	}
	
	@IBAction func deleteInstrument(sender: AnyObject!) {
		
	}
	
	@IBAction func playInstrument(sender: AnyObject!) {
		
	}
	
	@IBAction func showInstrumentInfo(sender: AnyObject!) {
		
	}
	
	@IBAction func toggleInfo(sender: AnyObject!) {
		
	}
	
	@IBAction func deleteSample(sender: AnyObject!) {
		
	}
	
	@IBAction func playSample(sender: AnyObject!) {
		
	}
}
