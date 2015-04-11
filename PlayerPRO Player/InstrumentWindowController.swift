//
//  PPInstrumentWindowController.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/23/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit
import CoreGraphics

class InstrumentWindowController: NSWindowController, NSOutlineViewDataSource, NSOutlineViewDelegate {
	@IBOutlet weak var infoDrawer:			NSDrawer!
	@IBOutlet weak var instrumentSize:		NSTextField!
	@IBOutlet weak var instrumentLoopStart:	NSTextField!
	@IBOutlet weak var instrumentLoopSize:	NSTextField!
	@IBOutlet weak var instrumentVolume:	NSTextField!
	@IBOutlet weak var instrumentRate:		NSTextField!
	@IBOutlet weak var instrumentNote:		NSTextField!
	@IBOutlet weak var instrumentBits:		NSTextField!
	@IBOutlet weak var instrumentMode:		NSTextField!
	@IBOutlet weak var waveFormImage:		NSImageView!
	@IBOutlet weak var instrumentView:		NSOutlineView!
	
	@IBAction func toggleInfo(sender: AnyObject?) {
		infoDrawer?.toggle(sender)
	}
	
	class func newInstrumentWindow() -> Self {
		var ourself = self(windowNibName:"InstrumentWindow")
		
		(NSApplication.sharedApplication().delegate as! PlayerAppDelegate).addObserver(ourself, forKeyPath: "music", options: .New, context: nil)
		
		return ourself
	}
	
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		instrumentView?.selectRowIndexes(NSIndexSet(index:0), byExtendingSelection:false)
    }

	override func observeValueForKeyPath(keyPath: String, ofObject object: AnyObject, change: [NSObject : AnyObject], context: UnsafeMutablePointer<()>) {
		if (keyPath == "music") {
			instrumentView?.reloadData()
		}
	}
	
	func outlineViewSelectionDidChange(notification: NSNotification) {
		var object: AnyObject? = instrumentView!.itemAtRow(instrumentView!.selectedRow)
		
		if object == nil {
			return
		}
		
		if let unwrapped = object as? PPInstrumentObject {
			if (unwrapped.countOfSamples > 0) {
				object = unwrapped.samplesObjectAtIndex(0)
			} else {
				object = nil
			}
		}
		
		if !(object! is PPSampleObject) {
			object = nil
		}
		
		if (object == nil) {
			instrumentSize!.stringValue = PPDoubleDash
			instrumentLoopStart!.stringValue = PPDoubleDash
			instrumentLoopSize!.stringValue = PPDoubleDash
			instrumentVolume!.stringValue = PPDoubleDash
			instrumentRate!.stringValue = PPDoubleDash
			instrumentNote!.stringValue = PPDoubleDash
			instrumentBits!.stringValue = PPDoubleDash
			instrumentMode!.stringValue = PPDoubleDash
			waveFormImage!.image = nil
			return;
		} else {
			var sampleObj = object as! PPSampleObject
			
			instrumentSize!.integerValue = sampleObj.data.length
			instrumentLoopStart!.integerValue = Int(sampleObj.loopBegin)
			instrumentLoopSize!.integerValue = Int(sampleObj.loopSize)
			instrumentVolume!.integerValue = Int(sampleObj.volume)
			instrumentRate!.stringValue = "\(sampleObj.c2spd) Hz"
			instrumentNote!.stringValue = octaveNameFromNote(UInt8(sampleObj.relativeNote)) ?? "None"
			instrumentBits!.stringValue = "\(sampleObj.amplitude)-bit"
			instrumentMode!.stringValue = sampleObj.loopType == .PingPong ? "Ping-Pong" : "Classic"
			let sampImage = sampleObj.waveformImage(view: waveFormImage!)
			waveFormImage!.image = sampImage
		}
	}
	
	func outlineView(outlineView: NSOutlineView, numberOfChildrenOfItem item: AnyObject?) -> Int {
		if (item == nil) {
			return (NSApplication.sharedApplication().delegate as! PlayerAppDelegate).music.instruments.count
		}
		if let item = item as? PPInstrumentObject {
			return item.countOfSamples
		}
		return 0
	}
	
	func outlineView(outlineView: NSOutlineView, child index: Int, ofItem item: AnyObject?) -> AnyObject {
		if (item == nil) {
			return (NSApplication.sharedApplication().delegate as! PlayerAppDelegate).music.instruments[index];
		}
		if let item = item as? PPInstrumentObject {
			return (item).samplesObjectAtIndex(index)
		}
		return NSNull();
	}
	
	func outlineView(outlineView: NSOutlineView, objectValueForTableColumn tableColumn: NSTableColumn?, byItem item: AnyObject?) -> AnyObject? {
		var toRet: String? = nil
		if let item = item as? PPInstrumentObject {
			toRet = item.name
		} else if let item = item as? PPSampleObject {
			toRet = item.name
		}
		return toRet
	}
	
	func outlineView(outlineView: NSOutlineView, isItemExpandable item: AnyObject) -> Bool {
		if let item = item as? PPInstrumentObject {
			return item.countOfSamples != 0
		}
		return false
	}
	
	deinit {
		NSNotificationCenter.defaultCenter().removeObserver(self)
		(NSApplication.sharedApplication().delegate as! PlayerAppDelegate).removeObserver(self, forKeyPath: "music")
	}
}
