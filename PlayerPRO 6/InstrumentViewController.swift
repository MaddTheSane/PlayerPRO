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

	@IBAction func playSample(sender: AnyObject!) {
	#if false
	let tag = sender.tag
	let sampNum = tag % MAXSAMPLE
	let instrNum = tag / MAXSAMPLE
	#endif
	}
	
	private func loadInstrumentsFromMusic() {
		if (instrumentOutline != nil) {
			instrumentOutline.reloadData()
			instrumentOutline.selectRowIndexes(NSIndexSet(index: 0), byExtendingSelection: false)
			instrumentOutline.scrollToBeginningOfDocument(nil)
		}
	}

	func importSampleFromURL(sampURL: NSURL, makeUserSelectInstrument selIns: Bool = false, error theErr: NSErrorPointer = nil) -> Bool {
		//TODO: handle selIns
		var plugType: MADFourChar = 0;
		var theOSErr = importer.identifyInstrumentFile(sampURL, type: &plugType)
		if (theOSErr != MADErr.NoErr) {
			if (theErr != nil) {
				theErr.memory = CreateErrorFromMADErrorType(theOSErr);
			}
			return false;
		};
		var theSamp: Int16 = 0;
		var theIns: Int16  = 0;
		
		/*
		InstrData *tmpInstr = &[currentDocument.wrapper internalMadMusicStruct]->fid[theIns];
		sData **tmpsData = &[currentDocument.wrapper internalMadMusicStruct]->sample[theIns];
		theOSErr = [importer importInstrumentOfType:plugType instrumentReference:tmpInstr sampleReference:tmpsData sample:&theSamp URL:sampURL plugInfo:NULL];
		if (theOSErr != MADNoErr) {
		if (theErr)
		*theErr = PPCreateErrorFromMADErrorType(theOSErr);
		
		return NO;
		} else {
		if (theErr)
		*theErr = nil;
		
		PPInstrumentObject *insObj = [[PPInstrumentObject alloc] initWithMusic:currentDocument.wrapper instrumentIndex:theIns];
		[self replaceObjectInInstrumentsAtIndex:theIns withObject:insObj];
		[instrumentView reloadData];
		//(*curMusic)->hasChanged = TRUE;
		return YES;
		}
		*/
		return false
	}

	
	func exportInstrumentListToURL(outURL: NSURL) -> MADErr {
		return currentDocument.wrapper.exportInstrumentListToURL(outURL)
	}

	func importInstrumentListFromURL(insURL: NSURL, error theErr: NSErrorPointer = nil) -> Bool {
		return currentDocument.wrapper.importInstrumentListFromURL(insURL, error: theErr)
	}
	
	@IBAction func importInstrument(sender: AnyObject!) {
		let plugCount = importer.plugInCount
		var fileDict = [String: [String]]()
		for obj in importer {
			fileDict[obj.menuName] = (obj.UTITypes as [String]);
		}
		let openPanel = NSOpenPanel()
		var vc = OpenPanelViewController(openPanel: openPanel, instrumentDictionary:fileDict)
		vc.setupDefaults()
		openPanel.beginSheetModalForWindow(currentDocument.windowForSheet, completionHandler: { (panelHandle) -> Void in
			if panelHandle == NSFileHandlingPanelOKButton {
				var err: NSError? = nil
				if self.importSampleFromURL(openPanel.URL, error: &err) == false {
					NSAlert(error:err!).beginSheetModalForWindow(self.currentDocument.windowForSheet, completionHandler: { (returnCode) -> Void in
						//do nothing
						return
					})
				}
			}
		})
	}
	
	func playSample(fromInstrument: Int16, sampleNumber: Int16, volume: Byte = 0xFF, note: Byte = 0xFF) {
		
	}

	override func awakeFromNib() {
		super.awakeFromNib()
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		instrumentOutline.selectRowIndexes(NSIndexSet(index: 0), byExtendingSelection: false)
		theDriver = currentDocument.theDriver;
	}

	
	@IBAction func exportInstrument(sender: AnyObject!) {
		
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
	
	@objc func outlineViewSelectionDidChange(notification: NSNotification!) {
		var object: AnyObject! = instrumentOutline.itemAtRow(instrumentOutline.selectedRow)
		
		func updateOutlineView(obj: PPSampleObject?) {
			if obj == nil {
				instrumentSize.stringValue = PPDoubleDash
				instrumentLoopStart.stringValue = PPDoubleDash
				instrumentLoopSize.stringValue = PPDoubleDash
				instrumentVolume.stringValue = PPDoubleDash
				instrumentRate.stringValue = PPDoubleDash
				instrumentNote.stringValue = PPDoubleDash
				instrumentBits.stringValue = PPDoubleDash
				instrumentMode.stringValue = PPDoubleDash
				self.waveFormImage.image = nil;
			} else {
				let untmpObj = obj!
				instrumentSize.integerValue = untmpObj.data.length
				instrumentLoopStart.integerValue = Int(untmpObj.loopBegin)
				instrumentLoopSize.integerValue = Int(untmpObj.loopSize)
				instrumentVolume.integerValue = Int(untmpObj.volume)
				instrumentRate.stringValue = "\(untmpObj.c2spd) Hz"
				instrumentNote.stringValue = PPSampleObject.octaveNameFromNote(Int16(untmpObj.relativeNote))
				instrumentBits.stringValue = "\(untmpObj.amplitude)-bit"
				instrumentMode.stringValue = untmpObj.loopType == .ePingPongLoop ? "Ping-pong" : "Classic"
				let tmpIm = PPSampleObject.waveformImage(fromSample: untmpObj, view: waveFormImage)
				waveFormImage.image = tmpIm
			}
		}
		
		if let otherObj = object as? PPInstrumentObject {
			if otherObj.countOfSamples > 0 {
				let tmpObj = otherObj.samplesObjectAtIndex(0)
				updateOutlineView(tmpObj)
			}
		} else if let otherObj = object as? PPSampleObject {
			updateOutlineView(otherObj)
		} else {
			updateOutlineView(nil)
		}
	}
	
	func outlineView(outlineView: NSOutlineView!, numberOfChildrenOfItem item: AnyObject!) -> Int {
		if item == nil {
			return currentDocument.wrapper.instruments.count
		}
		if let obj = item as? PPInstrumentObject {
			return obj.countOfSamples
		}
		
		return 0
	}
	
	func outlineView(outlineView: NSOutlineView!, child index: Int, ofItem item: AnyObject!) -> AnyObject! {
		if item == nil {
			return currentDocument.wrapper.instruments[index]
		}
		if let obj = item as? PPInstrumentObject {
			return obj.samplesObjectAtIndex(UInt(index))
		}
		return nil
	}
	
	func outlineView(outlineView: NSOutlineView!, isItemExpandable item: AnyObject!) -> Bool {
		if let obj = item as? PPInstrumentObject {
			return obj.countOfSamples != 0
		}
		return false
	}
	
	func outlineView(outlineView: NSOutlineView!, viewForTableColumn tableColumn: NSTableColumn!, item: AnyObject!) -> NSView! {
		let theView = outlineView.makeViewWithIdentifier(tableColumn.identifier, owner: nil) as PPInstrumentCellView!
		theView.controller = self
		if let obj = item as? PPInstrumentObject {
			theView.isSample = false
			theView.textField.stringValue = obj.name
			theView.numField.stringValue = NSString(format:"%03ld", obj.number + 1)
			theView.isBlank = obj.countOfSamples <= 0;
		} else if let obj = item as? PPSampleObject {
			theView.isSample = true
			theView.textField.stringValue = obj.name
			if item.loopSize != 0 {
				theView.isLoopingSample = true
			} else {
				theView.isLoopingSample = false
			}
			theView.sampleButton.tag = obj.instrumentIndex * Int(MAXSAMPLE) + obj.sampleIndex
			theView.isBlank = false
		}
		return theView
	}
	
	func replaceObjectInInstrumentsAtIndex(index: Int, withObject object: AnyObject!) {
		currentDocument.wrapper.instruments[index] = object;
		//(*curMusic)->hasChanged = TRUE;
	}
	
}