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
	
	init(window: NSWindow) {
		super.init(window: window)
		
		(NSApplication.sharedApplication().delegate as AppDelegate).addObserver(self, forKeyPath: "music", options: .New, context: nil)
	}
	
	/*
	convenience init() {
		super.init(windowNibName: "InstrumentWindow")
	}
	*/
	
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		instrumentView?.selectRowIndexes(NSIndexSet(index:0), byExtendingSelection:false)
    }

	public class func drawCGSampleInt(start startI: Int, tSS: Int, tSE: Int, high: Int, larg: Int, trueV: Int, trueH: Int, channel: Int16, curData: PPSampleObject!, ctxRef: CGContextRef!) {
		CGContextSaveGState(ctxRef);
		
		var start:Int = startI
		
		var i: Int = 0;
		var sampleSize = curData.data.length;
		var	temp: CGFloat = 0;
		var theSample = ConstUnsafePointer<UInt8>(curData.data.bytes)
		var theShortSample = ConstUnsafePointer<UInt16>(curData.data.bytes)
		var BS: Int = 0
		var BE: Int = 0
		var x: Int = 0
		var isStereo = curData.stereo;
		var minY: CGFloat = 0
		var maxY: CGFloat = 0
		
		if (curData.amplitude == 16) {
			sampleSize /= 2
			start /= 2
			
			BS = start + (tSS * sampleSize) / larg;
			if (isStereo) {
				BS /= 2;
				BS *= 2;
				BS += Int(channel);
			}
			temp = CGFloat(Int(theShortSample[BS]) + 0x8000);
			temp *= CGFloat(high);
			temp  /= CGFloat((1 << 16));
			CGContextMoveToPoint(ctxRef, CGFloat(trueH + tSS), trueV + temp);
			
			for (i = tSS; i < tSE; i++) {
				BS = start + (i * sampleSize) / larg;
				BE = start + ((i + 1) * sampleSize) / larg;
				
				if (isStereo) {
					BS /= 2;
					BS *= 2;
					BE /= 2;
					BE *= 2;
					
					BS += Int(channel);
					BE += Int(channel);
				}
				
				temp = CGFloat(Int(theShortSample[BS]) + 0x8000);
				minY = CGFloat(temp);
				maxY = CGFloat(temp);
				temp *= CGFloat(high);
				temp /= CGFloat((1 << 16));
				CGContextAddLineToPoint(ctxRef, CGFloat(trueH + i), CGFloat(temp + trueV));
				
				if (BS != BE) {
					for (x = BS; x < BE; x++) {
						temp = CGFloat(Int(theShortSample[x]) + 0x8000);
						
						if (temp > maxY) {
							maxY = temp;
						}
						
						if (temp < minY) {
							minY = temp;
						}
						
						if (isStereo) {
							x++;
						}
					}
					
					maxY *= CGFloat(high);
					maxY /= CGFloat((1 << 16));
					minY *= CGFloat(high);
					minY /= CGFloat((1 << 16));
					
					CGContextMoveToPoint(ctxRef, CGFloat(Double(trueH + i)), CGFloat(Double(minY + trueV)));
					CGContextAddLineToPoint(ctxRef, CGFloat(Double(trueH + i)), CGFloat(Double(maxY + trueV)));
				}
			}
		} else {
			BS = start + (tSS * sampleSize) / larg;
			if (isStereo) {
				BS /= 2;
				BS *= 2;
				BS += Int(channel);
			}
			
			temp = CGFloat((theSample[BS] - 0x80));
			temp *= CGFloat(high);
			temp /= CGFloat((1 << 8));
			
			CGContextMoveToPoint(ctxRef, CGFloat(trueH + tSS), trueV + temp);
			
			for (i = tSS; i < tSE; i++) {
				BS = start + (i * sampleSize) / larg;
				BE = start + ((i + 1) * sampleSize) / larg;
				
				if (isStereo) {
					BS /= 2;
					BS *= 2;
					
					BE /= 2;
					BE *= 2;
					
					BS += Int(channel);
					BE += Int(channel);
				}
				
				temp = CGFloat(theSample[BS] - 0x80);
				minY = temp; maxY = temp;
				temp *= CGFloat(high);
				temp /= CGFloat(1 << 8);
				CGContextAddLineToPoint(ctxRef, CGFloat(trueH + i), temp + trueV);
				
				if (BS != BE) {
					for (x = BS; x < BE; x++) {
						temp = CGFloat(theSample[x] - 0x80);
						
						if (temp > maxY) {
							maxY = temp;
						}
						if (temp < minY) {
							minY = temp;
						}
						
						if (isStereo) {
							x++;
						}
					}
					maxY *= CGFloat(high);
					maxY /= CGFloat(1 << 8);
					minY *= CGFloat(high);
					minY /= CGFloat(1 << 8);
					
					CGContextMoveToPoint(ctxRef, CGFloat(trueH + i), minY + trueV);
					CGContextAddLineToPoint(ctxRef, CGFloat(trueH + i), maxY + trueV);
				}
			}
		}
		CGContextStrokePath(ctxRef);
		CGContextRestoreGState(ctxRef);
	}
	
	private func waveformImageFromSample(theDat:PPSampleObject!) -> NSImage? {
		//This code causes the Swift compiler to crash.
		
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
			PPInstrumentWindowController.drawCGSampleInt(start: 0, tSS: 0, tSE: Int(imageSize.width), high: Int(imageSize.height), larg: Int(imageSize.width), trueV: 0, trueH: 0, channel: 1, curData: theDat, ctxRef: bitmapContext)
		}
		let stereoTrans: CGFloat = datIsStereo ? 0.75 : 1
		
		let colorRef2 = CGColorCreateGenericRGB(1, 0, 0, stereoTrans);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef2);
		PPInstrumentWindowController.drawCGSampleInt(start: 0, tSS: 0, tSE: Int(imageSize.width), high: Int(imageSize.height), larg: Int(imageSize.width), trueV: 0, trueH: 0, channel: 0, curData: theDat, ctxRef: bitmapContext)
		
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

	override func observeValueForKeyPath(keyPath: String!, ofObject object: AnyObject!, change: [NSObject : AnyObject]!, context: UnsafePointer<()>) {
		if (keyPath == "music") {
			instrumentView?.reloadData()
		}
	}
	
	func outlineViewSelectionDidChange(notification: NSNotification!) {
		var object: AnyObject? = instrumentView!.itemAtRow(instrumentView!.selectedRow)
		
		if (object?.isKindOfClass(PPInstrumentObject)) {
			if ((object as PPInstrumentObject).countOfSamples() > 0) {
				object = (object as PPInstrumentObject).samplesObjectAtIndex(0)
			} else {
				object = nil
			}
		}
		
		if (!object?.isKindOfClass(PPSampleObject)) {
			object = nil
		}
		
		if (!object) {
			((instrumentSize?) as NSTextField ).stringValue = PPDoubleDash
			((instrumentLoopStart?) as NSTextField ).stringValue = PPDoubleDash
			((instrumentLoopSize?) as NSTextField ).stringValue = PPDoubleDash
			((instrumentVolume?) as NSTextField ).stringValue = PPDoubleDash
			((instrumentRate?) as NSTextField ).stringValue = PPDoubleDash
			((instrumentNote?) as NSTextField ).stringValue = PPDoubleDash
			((instrumentBits?) as NSTextField ).stringValue = PPDoubleDash
			((instrumentMode?) as NSTextField ).stringValue = PPDoubleDash
			((waveFormImage?) as NSImageView ).image = nil
			return;
		} else {
		var sampleObj = object as PPSampleObject
		
		((instrumentSize?) as NSTextField ).integerValue = sampleObj.data.length
		((instrumentLoopStart?) as NSTextField ).integerValue = Int(sampleObj.loopBegin)
		((instrumentLoopSize?) as NSTextField ).integerValue = Int(sampleObj.loopSize)
		((instrumentVolume?) as NSTextField ).integerValue = Int(sampleObj.volume)
		((instrumentRate?) as NSTextField ).stringValue = "\(sampleObj.c2spd) Hz"
		((instrumentNote?) as NSTextField ).stringValue = "\(sampleObj.amplitude)" //TODO: properly set note.
		((instrumentBits?) as NSTextField ).stringValue = "\(sampleObj.amplitude)-bit"
		((instrumentMode?) as NSTextField ).stringValue = sampleObj.loopType == .ePingPongLoop ? "Ping-Pong" : "Classic"
		var sampImage = waveformImageFromSample(sampleObj)
		((waveFormImage?) as NSImageView ).image = sampImage
		}
	}
	
	func outlineView(outlineView: NSOutlineView!, numberOfChildrenOfItem item: AnyObject!) -> Int {
		if (!item) {
			return (NSApplication.sharedApplication().delegate as AppDelegate).music.instruments().count
		}
		if (item.isKindOfClass(PPInstrumentObject)) {
			return Int((item as PPInstrumentObject).countOfSamples())
		}
		return 0
	}
	
	func outlineView(outlineView: NSOutlineView!, objectValueForTableColumn tableColumn: NSTableColumn!, byItem item: AnyObject!) -> AnyObject! {
		if (item.isKindOfClass(PPInstrumentObject)) {
			return (item as PPInstrumentObject).name
		} else if (item.isKindOfClass(PPSampleObject)) {
			return (item as PPSampleObject).name
		}
		return nil
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
