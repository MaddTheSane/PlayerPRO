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

class PPInstrumentWindowController: NSWindowController, NSOutlineViewDataSource, NSOutlineViewDelegate {
	@IBOutlet var infoDrawer: NSDrawer?
	@IBOutlet var instrumentSize: NSTextField?
	@IBOutlet var instrumentLoopStart: NSTextField?
	@IBOutlet var instrumentLoopSize: NSTextField?
	@IBOutlet var instrumentVolume: NSTextField?
	@IBOutlet var instrumentRate: NSTextField?
	@IBOutlet var instrumentNote: NSTextField?
	@IBOutlet var instrumentBits: NSTextField?
	@IBOutlet var instrumentMode: NSTextField?
	@IBOutlet var waveFormImage: NSImageView?
	@IBOutlet var instrumentView: NSOutlineView?
	
	@IBAction func toggleInfo(sender: AnyObject?) {
		infoDrawer?.toggle(sender)
	}
	
	class func newInstrumentWindow() -> Self {
		var ourself = self(windowNibName:"InstrumentWindow")
		
		(NSApplication.sharedApplication().delegate as AppDelegate).addObserver(ourself, forKeyPath: "music", options: .New, context: nil)
		
		return ourself
	}
	
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		instrumentView?.selectRowIndexes(NSIndexSet(index:0), byExtendingSelection:false)
    }

	private func waveformImageFromSample(theDat:PPSampleObject!) -> NSImage? {
		var imageSize = waveFormImage!.convertSizeToBacking(waveFormImage!.frame.size)
		let datIsStereo = theDat.stereo;
		imageSize.height *= 2;
		imageSize.width *= 2;
		var theCGimg: CGImageRef? = nil
		let rowBytes: UInt = 4 * UInt(imageSize.width)
		let defaultSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB)!
		let bitMapFormat = CGBitmapInfo.fromRaw( CGImageAlphaInfo.PremultipliedLast.toRaw())
		var bitmapContext = CGBitmapContextCreate(nil, UInt(imageSize.width), UInt(imageSize.height), 8, rowBytes, defaultSpace, bitMapFormat!);
		CGContextClearRect(bitmapContext, CGRectMake(0, 0, imageSize.width, imageSize.height));
		let lineSize = waveFormImage!.convertSizeToBacking(NSMakeSize(1, 1));
		CGContextSetLineWidth(bitmapContext, lineSize.height);
		if (datIsStereo) {
			let colorRef1 = CGColorCreateGenericRGB(0, 0, 1, 0.75);
			CGContextSetStrokeColorWithColor(bitmapContext, colorRef1);
			PPSampleObject.drawCGSampleInt(tSE: Int(imageSize.width), high: Int(imageSize.height), larg: Int(imageSize.width), channel: 1, curData: theDat, ctxRef: bitmapContext)
		}
		let stereoTrans: CGFloat = datIsStereo ? 0.75 : 1
		
		let colorRef2 = CGColorCreateGenericRGB(1, 0, 0, stereoTrans);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef2);
		PPSampleObject.drawCGSampleInt(tSE: Int(imageSize.width), high: Int(imageSize.height), larg: Int(imageSize.width), channel: 0, curData: theDat, ctxRef: bitmapContext)
		
		if (theDat.loopSize != 0) {
			let colorRef3 = CGColorCreateGenericRGB(1, 0.1, 0.5, 0.8);
			CGContextSetStrokeColorWithColor(bitmapContext, colorRef3);
			var loopRect = CGRectMake(0, 0, imageSize.width, imageSize.height);
			var lineSize = waveFormImage!.convertSizeToBacking(NSMakeSize(2, 2));
			var padSize = waveFormImage!.convertSizeToBacking(NSMakeSize(1, 1));
			CGContextSetLineWidth(bitmapContext, lineSize.height);
			loopRect.origin.x =  (CGFloat(Double(theDat.loopBegin)) * imageSize.width / CGFloat(Double(theDat.data.length)));
			loopRect.origin.y += padSize.width;
			loopRect.size.width = CGFloat(Double(theDat.loopSize)) * imageSize.width / CGFloat(Double(theDat.data.length))
			loopRect.size.height -= padSize.width * 2;
			CGContextStrokeRect(bitmapContext, loopRect);
		}
		
		theCGimg = CGBitmapContextCreateImage(bitmapContext);
		
		return NSImage(CGImage: theCGimg, size: waveFormImage!.frame.size)
	}

	override func observeValueForKeyPath(keyPath: String!, ofObject object: AnyObject!, change: [NSObject : AnyObject]!, context: UnsafeMutablePointer<()>) {
		if (keyPath == "music") {
			instrumentView?.reloadData()
		}
	}
	
	func outlineViewSelectionDidChange(notification: NSNotification!) {
		var object: AnyObject? = instrumentView!.itemAtRow(instrumentView!.selectedRow)
		
		if (object?.isKindOfClass(PPInstrumentObject) == true) {
			if ((object as PPInstrumentObject).countOfSamples() > 0) {
				object = (object as PPInstrumentObject).samplesObjectAtIndex(0)
			} else {
				object = nil
			}
		}
		
		if (object?.isKindOfClass(PPSampleObject) == false) {
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
			var sampleObj = object as PPSampleObject
			
			instrumentSize!.integerValue = sampleObj.data.length
			instrumentLoopStart!.integerValue = Int(sampleObj.loopBegin)
			instrumentLoopSize!.integerValue = Int(sampleObj.loopSize)
			instrumentVolume!.integerValue = Int(sampleObj.volume)
			instrumentRate!.stringValue = "\(sampleObj.c2spd) Hz"
			instrumentNote!.stringValue = "\(sampleObj.amplitude)" //TODO: properly set note.
			instrumentBits!.stringValue = "\(sampleObj.amplitude)-bit"
			instrumentMode!.stringValue = sampleObj.loopType == .ePingPongLoop ? "Ping-Pong" : "Classic"
			var sampImage = waveformImageFromSample(sampleObj)
			waveFormImage!.image = sampImage
		}
	}
	
	func outlineView(outlineView: NSOutlineView!, numberOfChildrenOfItem item: AnyObject!) -> Int {
		if (!item) {
			return (NSApplication.sharedApplication().delegate as AppDelegate).music.instruments.count
		}
		if (item.isKindOfClass(PPInstrumentObject)) {
			return Int((item as PPInstrumentObject).countOfSamples())
		}
		return 0
	}
	
	func outlineView(outlineView: NSOutlineView!, child index: Int, ofItem item: AnyObject!) -> AnyObject! {
		if (!item) {
			return (NSApplication.sharedApplication().delegate as AppDelegate).music.instruments[index];
		}
		if (item.isKindOfClass(PPInstrumentObject)) {
			return (item as PPInstrumentObject).samplesObjectAtIndex(UInt(index))
		}
		return nil;
	}
	
	func outlineView(outlineView: NSOutlineView!, objectValueForTableColumn tableColumn: NSTableColumn!, byItem item: AnyObject!) -> AnyObject! {
		var toRet: String? = nil
		if (item.isKindOfClass(PPInstrumentObject)) {
			toRet = (item as PPInstrumentObject).name
		} else if (item.isKindOfClass(PPSampleObject)) {
			toRet = (item as PPSampleObject).name
		}
		return toRet
	}
	
	func outlineView(outlineView: NSOutlineView!, isItemExpandable item: AnyObject!) -> Bool {
		if (item.isKindOfClass(PPInstrumentObject)) {
			return (item as PPInstrumentObject).countOfSamples() != 0
		}
		return false
	}
	
	deinit {
		NSNotificationCenter.defaultCenter().removeObserver(self)
		(NSApplication.sharedApplication().delegate as AppDelegate).removeObserver(self, forKeyPath: "music")
	}
}
