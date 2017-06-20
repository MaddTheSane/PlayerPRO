//
//  ComplexFadeController.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/18/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit

private func StringToHex(_ str: String) -> Int32 {
	let tmpScanner = Scanner(string: str)
	var tmpVal: UInt32  = 0;
	if !tmpScanner.scanHexInt32(&tmpVal) {
		return -1;
	} else if (tmpVal == UInt32.max || tmpVal > 0xFF) {
		return -1;
	}
	
	return Int32(tmpVal);
}

enum PPFadeType: Int {
	case instrument = 1
	case note
	case argument
	case volume
}

class ComplexFadeController: NSWindowController {
	@IBOutlet weak var stepCell: NSFormCell!
	@IBOutlet weak var fromCell: NSFormCell!
	@IBOutlet weak var toCell: NSFormCell!
	
	@IBOutlet weak var fadeTypeMatrix: NSMatrix!
	@IBOutlet weak var valueInfo: NSTextField!
	var fadeType = PPFadeType.instrument
	var thePcmd: UnsafeMutablePointer<Pcmd>? = nil
	var currentBlock: PPPlugErrorBlock!
	
	var parentWindow: NSWindow!
	
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
    }

	@IBAction func changeFadeType(_ sender: AnyObject!) {
		fadeType = PPFadeType(rawValue: (sender as! NSButtonCell).tag)!
		switch (fadeType) {
		case .instrument:
			valueInfo.stringValue = NSLocalizedString("From 1 to 64", bundle: Bundle(for: ComplexFadeController.self), comment: "From 1 to 64")
			fromCell.placeholderString = "1";
			toCell.placeholderString = "64";
			
		case .argument:
			valueInfo.stringValue = NSLocalizedString("From C0 to B7", bundle: Bundle(for: ComplexFadeController.self), comment: "From C0 to B7")
			fromCell.placeholderString = "C0";
			toCell.placeholderString = "B7";
			
		case .note, .volume:
			valueInfo.stringValue = NSLocalizedString("From 00 to FF", bundle: Bundle(for: ComplexFadeController.self), comment: "From 00 to FF")
			fromCell.placeholderString = "00";
			toCell.placeholderString = "FF";
		}

	}
	
	@IBAction func okay(_ sender: AnyObject!) {
		func invalidSettings() {
			NSBeep()
			let badSettings = NSAlert()
			badSettings.messageText = "Invalid Value";
			badSettings.informativeText = "There is one or more invalid value.";
			badSettings.addButton(withTitle: "OK")
			
			badSettings.beginSheetModal(for: self.window!, completionHandler: { (returnCode) -> Void in
				
			})
		}
		
		var from: Int32 = 0
		var to: Int32 = 0
		var step = 0
		let numFormatter = NumberFormatter()
		var ourUnknown: AnyObject? = nil
		
		switch fadeType {
		case .instrument:
			if numFormatter.getObjectValue(&ourUnknown, for: fromCell.stringValue, errorDescription: nil) {
				if let aNumber = ourUnknown as? NSNumber, let bNumber = aNumber as? Int {
					from = Int32(bNumber)
				} else {
					invalidSettings()
					return
				}
			} else {
				invalidSettings()
				return
			}
			
			if numFormatter.getObjectValue(&ourUnknown, for: toCell.stringValue, errorDescription: nil) {
				if let aNumber = ourUnknown as? NSNumber, let bNumber = aNumber as? Int {
					to = Int32(bNumber)
				} else {
					invalidSettings()
					return
				}
			} else {
				invalidSettings()
				return
			}
			
			if from < 1 || from > 64 {
				invalidSettings()
				return
			}
			if to < 1 || to > 64 {
				invalidSettings()
				return
			}
			
		case .note:
			let tmpFrom: Int16 = note(from: fromCell.stringValue) ?? 0xFF
			let tmpTo: Int16 = note(from: toCell.stringValue) ?? 0xFF
			from = Int32(tmpFrom)
			to = Int32(tmpTo)
			
			if from < 0 || from >= 96 {
				invalidSettings()
				return
			}
			if to < 0 || to >= 96 {
				invalidSettings()
				return
			}
			
		case .argument, .volume:
			from = StringToHex(self.fromCell.stringValue);
			to = StringToHex(self.toCell.stringValue);
			
			if from < 0 || from > 0xFF {
				invalidSettings()
				return
			}
			if to < 0 || to > 0xFF {
				invalidSettings()
				return
			}
		}
		
		if numFormatter.getObjectValue(&ourUnknown, for: stepCell.stringValue, errorDescription: nil) {
			if let aNumber = ourUnknown as? Int {
				step = aNumber
			} else {
				invalidSettings()
				return
			}
		} else {
			invalidSettings()
			return
		}
		
		if step < 1 || step > 64 {
			invalidSettings()
			return
		}
		
		let tmpPPKPcmd = PPKPcmd(thePcmd!)
		
		let pcmdLength = tmpPPKPcmd.length
		for track in 0 ..< tmpPPKPcmd.tracks {
			for row in stride(from: Int16(0), through: pcmdLength, by: step) {
				let myCmd = MADGetCmd(row, track, thePcmd);
				
				if pcmdLength > 1 {			// no zero div !!
					switch fadeType {
					case .instrument:
						myCmd?.pointee.ins	= UInt8(from + ((to - from) * Int32(row)) / (Int32(pcmdLength) - 1))
						
					case .note:
						myCmd?.pointee.note	= UInt8(from + ((to - from) * Int32(row)) / (Int32(pcmdLength) - 1))
						
					case .argument:
						myCmd?.pointee.arg	= UInt8(from + ((to - from) * Int32(row)) / (Int32(pcmdLength) - 1))
						
					case .volume:
						myCmd?.pointee.vol	= UInt8(from + ((to - from) * Int32(row)) / (Int32(pcmdLength) - 1))
					}
				}
			}
		}
		
		parentWindow.endSheet(window!)
		currentBlock(.noErr)
	}
	
	@IBAction func cancel(_ sender: AnyObject!) {
		parentWindow.endSheet(window!)
		currentBlock(.userCanceledErr)
	}
}
