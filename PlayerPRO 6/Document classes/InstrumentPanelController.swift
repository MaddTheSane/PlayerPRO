//
//  InstrumentPanelController.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/29/14.
//
//

import Cocoa

class InstrumentPanelController: NSWindowController, NSOutlineViewDataSource, NSOutlineViewDelegate {
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
	@IBOutlet weak var instrumentOutline:	NSOutlineView!
	
	@IBOutlet weak var currentDocument: PPDocument!
	weak var importer: PPInstrumentPlugHandler!
	weak var filterHandler: PPFilterPlugHandler!
	weak var theDriver: PPDriver!
	
	func colorsDidChange(aNot: NSNotification) {
		
	}
	
	@IBAction func playSample(sender: AnyObject!) {
		let tag = (sender as NSButtonCell).tag
		let sampNum = tag % Int(MAXSAMPLE)
		let instrNum = tag / Int(MAXSAMPLE)
		playSample(instrument: Int16(instrNum), sample: Int16(sampNum))
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
				theErr.memory = CreateErrorFromMADErrorType(theOSErr)
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
		return currentDocument.theMusic.exportInstrumentListToURL(outURL)
	}
	
	func importInstrumentListFromURL(insURL: NSURL, error theErr: NSErrorPointer = nil) -> Bool {
		return currentDocument.theMusic.importInstrumentListFromURL(insURL, error: theErr)
	}
	
	@IBAction func importInstrument(sender: AnyObject!) {
		let plugCount = importer.plugInCount
		var fileDict = [String: [String]]()
		for obj in importer {
			fileDict[obj.menuName] = (obj.UTITypes as [String]);
		}
		let openPanel = NSOpenPanel()
		if let vc = OpenPanelViewController(openPanel: openPanel, instrumentDictionary:fileDict) {
			vc.setupDefaults()
			vc.beginOpenPanel(parentWindow: currentDocument.windowForSheet!, completionHandler: { (panelHandle) -> Void in
				if panelHandle == NSFileHandlingPanelOKButton {
					var err: NSError? = nil
					if self.importSampleFromURL(openPanel.URL!, error: &err) == false {
						NSAlert(error:err!).beginSheetModalForWindow(self.currentDocument.windowForSheet!, completionHandler: { (returnCode) -> Void in
							//do nothing
							return
						})
					}
				}
			})
		}
	}
	
	func playSample(#instrument: Int16, sample sampleNumber: Int16, volume: Byte = 0xFF, note: Byte = 0xFF) {
		
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		//instrumentOutline.selectRowIndexes(NSIndexSet(index: 0), byExtendingSelection: false)
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
				instrumentLoopStart.stringValue = ""
				instrumentLoopSize.stringValue = ""
				instrumentVolume.stringValue = PPDoubleDash
				instrumentRate.stringValue = PPDoubleDash
				instrumentNote.stringValue = PPDoubleDash
				instrumentBits.stringValue = PPDoubleDash
				instrumentMode.stringValue = PPDoubleDash
				self.waveFormImage.image = nil;
			} else {
				let untmpObj = obj!
				instrumentSize.integerValue = untmpObj.data.length
				if untmpObj.loopSize != 0 {
					instrumentLoopStart.integerValue = Int(untmpObj.loopBegin)
					instrumentLoopSize.integerValue = Int(untmpObj.loopSize)
				} else {
					instrumentLoopStart.stringValue = ""
					instrumentLoopSize.stringValue = ""
				}
				instrumentVolume.integerValue = Int(untmpObj.volume)
				instrumentRate.stringValue = untmpObj.c2spd != 0 ? "\(untmpObj.c2spd) Hz" : PPDoubleDash
				instrumentNote.stringValue = OctaveNameFromNote(UInt8(untmpObj.relativeNote)) ?? "---"
				instrumentBits.stringValue = untmpObj.amplitude != 0 ? "\(untmpObj.amplitude)-bit" : PPDoubleDash
				instrumentMode.stringValue = untmpObj.loopType == .PingPong ? "Ping-pong" : "Classic"
				let tmpIm = untmpObj.waveformImage(view: waveFormImage)
				waveFormImage.image = tmpIm
			}
		}
		
		if let otherObj = object as? PPInstrumentObject {
			if otherObj.countOfSamples > 0 {
				let tmpObj = otherObj.samplesObjectAtIndex(0)
				updateOutlineView(tmpObj)
			} else {
				updateOutlineView(nil)
				return
			}
		} else if let otherObj = object as? PPSampleObject {
			updateOutlineView(otherObj)
		} else {
			updateOutlineView(nil)
		}
	}
	
	func outlineView(outlineView: NSOutlineView!, numberOfChildrenOfItem item: AnyObject!) -> Int {
		if item == nil {
			return currentDocument.theMusic.instruments.count
		}
		if let obj = item as? PPInstrumentObject {
			return obj.countOfSamples
		}
		
		return 0
	}
	
	func outlineView(outlineView: NSOutlineView!, child index: Int, ofItem item: AnyObject!) -> AnyObject! {
		if item == nil {
			return currentDocument.theMusic.instruments[index]
		}
		if let obj = item as? PPInstrumentObject {
			return obj.samplesObjectAtIndex(index)
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
			theView.sample = false
			theView.textField!.stringValue = obj.name
			theView.numField.stringValue = NSString(format:"%03ld", obj.number + 1)
			theView.blank = obj.countOfSamples <= 0;
		} else if let obj = item as? PPSampleObject {
			theView.sample = true
			theView.textField!.stringValue = obj.name
			if item.loopSize != 0 {
				theView.loopingSample = true
			} else {
				theView.loopingSample = false
			}
			theView.sampleButton.tag = obj.instrumentIndex * Int(MAXSAMPLE) + obj.sampleIndex
			theView.blank = false
		}
		return theView
	}
	
	func replaceObjectInInstrumentsAtIndex(index: Int, withObject object: PPInstrumentObject!) {
		currentDocument.theMusic.replaceObjectInInstrumentsAtIndex(index, withObject: object)
	}
	
}
