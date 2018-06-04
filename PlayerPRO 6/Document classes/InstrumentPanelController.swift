//
//  InstrumentPanelController.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/29/14.
//
//

import Cocoa
import PlayerPROKit

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
	weak var instrumentImporter: PPInstrumentPlugHandler!
	weak var sampleImporter: SamplePlugHandler!
	weak var filterHandler: FilterPlugHandler!
	weak var theDriver: PPDriver!
	
	func colorsDidChange(_ aNot: Notification) {
		
	}
	
	@IBAction func playSample(_ sender: AnyObject!) {
		let tag = (sender as! NSButtonCell).tag
		let sampNum = tag % Int(MAXSAMPLE)
		let instrNum = tag / Int(MAXSAMPLE)
		playSample(instrument: Int16(instrNum), sample: Int16(sampNum))
	}
	
	private func loadInstrumentsFromMusic() {
		if (instrumentOutline != nil) {
			instrumentOutline.reloadData()
			instrumentOutline.selectRowIndexes(IndexSet(integer: 0), byExtendingSelection: false)
			instrumentOutline.scrollToBeginningOfDocument(nil)
		}
	}
	
	func importSample(from sampURL: URL, makeUserSelectInstrument selIns: Bool = false) throws {
		//TODO: handle selIns
		let plugType: MADFourChar = try sampleImporter.identifySampleFile(sampURL)
		var theSamp: Int16 = 0;
		var theIns: Int16  = 0;
		
		sampleImporter.beginImportingSample(type: plugType, URL: sampURL, driver: theDriver, parentDocument: currentDocument) { (err, obj) in
			if let err = err {
				self.currentDocument.presentError(err)
			} else if let obj = obj {
				self.currentDocument.theMusic.instruments.first?.add(obj)
			} else {
				
			}

		}
	}
	
	func importInstrument(from sampURL: URL, makeUserSelectInstrument selIns: Bool = false) throws {
		//TODO: handle selIns
		let plugType: MADFourChar = try instrumentImporter.identifyInstrumentFile(sampURL)
		var theSamp: Int16 = 0;
		var theIns: Int16  = 0;
		
		instrumentImporter.beginImportingInstrument(ofType: plugType, from: sampURL, driver: currentDocument.theDriver, parentDocument: currentDocument) { (err, obj) in
			if let err = err {
				self.currentDocument.presentError(err)
			} else if let obj = obj {
				self.replaceObjectInInstruments(at: Int(theIns), withObject: obj)
				self.instrumentOutline.reloadData()
			} else {
				
			}
		}
	}
	
	func exportInstrumentList(to outURL: URL) -> MADErr {
		return currentDocument.theMusic.exportInstrumentList(to: outURL)
	}
	
	func importInstrumentList(from insURL: URL) throws {
		try currentDocument.theMusic.importInstrumentList(from: insURL)
	}
	
	@IBAction func importInstrument(_ sender: AnyObject!) {
		var fileDict = [String: [String]]()
		for obj in instrumentImporter {
			fileDict[obj.menuName] = obj.utiTypes
		}
		for obj in sampleImporter {
			fileDict[obj.menuName] = obj.utiTypes
		}

		let openPanel = NSOpenPanel()
		if let vc = OpenPanelViewController(openPanel: openPanel, instrumentDictionary:fileDict) {
			vc.setupDefaults()
			vc.beginOpenPanel(currentDocument.windowForSheet!, completionHandler: { (panelHandle: NSApplication.ModalResponse) -> Void in
				if panelHandle.rawValue == NSFileHandlingPanelOKButton {
					do {
						_ = try self.instrumentImporter.identifyInstrumentFile(openPanel.url!)
						try self.importInstrument(from: openPanel.url!)
					} catch let error as NSError {
						if error.domain == PPMADErrorDomain && error.code == Int(MADErr.cannotFindPlug.rawValue) {
							// Try sample importing
							do {
								try self.importSample(from: openPanel.url!)
							} catch {
								self.currentDocument.presentError(error)
							}
						} else {
							//present error
							self.currentDocument.presentError(error)
						}
					}
				}
			})
		}
	}
	
	func playSample(instrument: Int16, sample sampleNumber: Int16, volume: UInt8 = 0xFF, note: UInt8 = 0xFF) {
		
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		//instrumentOutline.selectRowIndexes(NSIndexSet(index: 0), byExtendingSelection: false)
		theDriver = currentDocument.theDriver
		sampleImporter = (AppDelegate.shared as! AppDelegate).samplesHandler
		instrumentImporter = (AppDelegate.shared as! AppDelegate).instrumentPlugHandler
		filterHandler = (AppDelegate.shared as! AppDelegate).filterHandler
	}
	
	
	@IBAction func exportInstrument(_ sender: AnyObject!) {
		
	}
	
	@IBAction func deleteInstrument(_ sender: AnyObject!) {
		
	}
	
	@IBAction func playInstrument(_ sender: AnyObject!) {
		
	}
	
	@IBAction func showInstrumentInfo(_ sender: AnyObject!) {
		
	}
	
	@IBAction func toggleInfo(_ sender: AnyObject!) {
		
	}
	
	@IBAction func deleteSample(_ sender: AnyObject!) {
		
	}
	
	@objc func outlineViewSelectionDidChange(_ notification: Notification) {
		var object: AnyObject! = instrumentOutline.item(atRow: instrumentOutline.selectedRow) as AnyObject?
		
		func updateOutlineView(_ obj: PPSampleObject?) {
			if obj == nil {
				self.instrumentSize.stringValue = PPDoubleDash
				self.instrumentLoopStart.stringValue = ""
				self.instrumentLoopSize.stringValue = ""
				self.instrumentVolume.stringValue = PPDoubleDash
				self.instrumentRate.stringValue = PPDoubleDash
				self.instrumentNote.stringValue = PPDoubleDash
				self.instrumentBits.stringValue = PPDoubleDash
				self.instrumentMode.stringValue = PPDoubleDash
				self.waveFormImage.image = nil;
			} else {
				let untmpObj = obj!
				self.instrumentSize.integerValue = untmpObj.data.count
				if untmpObj.loopSize != 0 {
					self.instrumentLoopStart.integerValue = Int(untmpObj.loopBegin)
					self.instrumentLoopSize.integerValue = Int(untmpObj.loopSize)
				} else {
					self.instrumentLoopStart.stringValue = ""
					self.instrumentLoopSize.stringValue = ""
				}
				self.instrumentVolume.integerValue = Int(untmpObj.volume)
				self.instrumentRate.stringValue = untmpObj.c2spd != 0 ? "\(untmpObj.c2spd) Hz" : PPDoubleDash
				self.instrumentNote.stringValue = octaveName(fromNote: UInt8(untmpObj.realNote), options: [.useSharpSymbol]) ?? "---"
				self.instrumentBits.stringValue = untmpObj.amplitude != 0 ? "\(untmpObj.amplitude)-bit" : PPDoubleDash
				self.instrumentMode.stringValue = untmpObj.loopType == .pingPong ? "Ping-pong" : "Classic"
				let tmpIm = untmpObj.waveformImage(using: self.waveFormImage)
				self.waveFormImage.image = tmpIm
			}
		}
		
		if let otherObj = object as? PPInstrumentObject {
			if otherObj.countOfSamples > 0 {
				let tmpObj = otherObj.samplesObject(at: 0)
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
	
	func outlineView(_ outlineView: NSOutlineView, numberOfChildrenOfItem item: Any?) -> Int {
		if item == nil {
			return currentDocument.theMusic.instruments.count
		}
		if let obj = item as? PPInstrumentObject {
			return obj.countOfSamples
		}
		
		return 0
	}
	
	func outlineView(_ outlineView: NSOutlineView, child index: Int, ofItem item: Any?) -> Any {
		if item == nil {
			return currentDocument.theMusic.instruments[index]
		}
		if let obj = item as? PPInstrumentObject {
			return obj.samplesObject(at: index)
		}
		return NSNull()
	}
	
	func outlineView(_ outlineView: NSOutlineView, isItemExpandable item: Any) -> Bool {
		if let obj = item as? PPInstrumentObject {
			return obj.countOfSamples != 0
		}
		return false
	}
	
	func outlineView(_ outlineView: NSOutlineView, viewFor tableColumn: NSTableColumn?, item: Any) -> NSView? {
		if tableColumn == nil {
			return nil
		}
		let theView = outlineView.makeView(withIdentifier: tableColumn!.identifier, owner: nil) as! PPInstrumentCellView
		theView.controller = self
		if let obj = item as? PPInstrumentObject {
			theView.isSample = false
			theView.textField!.stringValue = obj.name
			theView.numField!.stringValue = String(format:"%03ld", obj.number + 1)
			theView.isBlank = obj.countOfSamples <= 0;
		} else if let obj2 = item as? PPSampleObject {
			theView.isSample = true
			theView.textField!.stringValue = obj2.name
			if (item as AnyObject).loopSize != 0 {
				theView.isLoopingSample = true
			} else {
				theView.isLoopingSample = false
			}
			theView.sampleButton!.tag = obj2.instrumentIndex * Int(MAXSAMPLE) + obj2.sampleIndex
			theView.isBlank = false
		}
		return theView
	}
	
	@objc(replaceObjectInInstrumentsAtIndex:withObject:)
	func replaceObjectInInstruments(at index: Int, withObject object: PPInstrumentObject!) {
		currentDocument.theMusic.replaceInInstruments(at: index, with: object)
	}
	
}
