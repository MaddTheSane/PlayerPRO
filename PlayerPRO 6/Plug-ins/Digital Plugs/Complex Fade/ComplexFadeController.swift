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

private func StringToHex(str: String) -> Int32 {
	var tmpScanner = NSScanner(string: str)
	var tmpVal: UInt32  = 0;
	if !tmpScanner.scanHexInt(&tmpVal) {
		return -1;
	} else if (tmpVal == UInt32.max || tmpVal > 0xFF) {
		return -1;
	}
	
	return Int32(tmpVal);
}

enum PPFadeType: Int {
	case Instrument = 1
	case Note
	case Argument
	case Volume
}

class ComplexFadeController: NSWindowController {
	@IBOutlet weak var stepCell: NSFormCell!
	@IBOutlet weak var fromCell: NSFormCell!
	@IBOutlet weak var toCell: NSFormCell!
	
	@IBOutlet weak var fadeTypeMatrix: NSMatrix!
	@IBOutlet weak var valueInfo: NSTextField!
	var fadeType = PPFadeType.Instrument
	var thePcmd: UnsafeMutablePointer<Pcmd> = nil
	var currentBlock: PPPlugErrorBlock!
	
	var parentWindow: NSWindow!
	
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
    }

	@IBAction func changeFadeType(sender: AnyObject!) {
		fadeType = PPFadeType(rawValue: (sender as! NSButtonCell).tag)!
		switch (fadeType) {
		case .Instrument:
			valueInfo.stringValue = NSLocalizedString("From 1 to 64", bundle: NSBundle(forClass: ComplexFadeController.self), comment: "From 1 to 64")
			fromCell.placeholderString = "1";
			toCell.placeholderString = "64";
			
		case .Argument:
			valueInfo.stringValue = NSLocalizedString("From C0 to B7", bundle: NSBundle(forClass: ComplexFadeController.self), comment: "From C0 to B7")
			fromCell.placeholderString = "C0";
			toCell.placeholderString = "B7";
			
		case .Note, .Volume:
			valueInfo.stringValue = NSLocalizedString("From 00 to FF", bundle: NSBundle(forClass: ComplexFadeController.self), comment: "From 00 to FF")
			fromCell.placeholderString = "00";
			toCell.placeholderString = "FF";
		}

	}
	
	@IBAction func okay(sender: AnyObject!) {
		func invalidSettings() {
			NSBeep()
			var badSettings = NSAlert()
			badSettings.messageText = "Invalid Value";
			badSettings.informativeText = "There is one or more invalid value.";
			badSettings.addButtonWithTitle("OK")
			
			badSettings.beginSheetModalForWindow(self.window!, completionHandler: { (returnCode) -> Void in
				
			})
		}
		
		var from: Int32 = 0
		var to: Int32 = 0
		var step = 0
		let numFormatter = NSNumberFormatter()
		var ourUnknown: AnyObject? = nil
		
		switch fadeType {
		case .Instrument:
			if numFormatter.getObjectValue(&ourUnknown, forString: fromCell.stringValue, errorDescription: nil) {
				if let aNumber = ourUnknown! as? NSNumber {
					from = Int32(aNumber as Int)
				} else {
					invalidSettings()
					return
				}
			} else {
				invalidSettings()
				return
			}
			
			if numFormatter.getObjectValue(&ourUnknown, forString: toCell.stringValue, errorDescription: nil) {
				if let aNumber = ourUnknown! as? NSNumber {
					to = Int32(aNumber as Int)
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
			
		case .Note:
			var tmpFrom: Int16 = noteFromString(fromCell.stringValue) ?? 0xFF
			var tmpTo: Int16 = noteFromString(toCell.stringValue) ?? 0xFF
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
			
		case .Argument, .Volume:
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
		
		if numFormatter.getObjectValue(&ourUnknown, forString: stepCell.stringValue, errorDescription: nil) {
			if let aNumber = ourUnknown! as? NSNumber {
				step = aNumber as Int
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
		
		let tmpPPKPcmd = PPKPcmd(thePcmd)
		
		let pcmdLength = tmpPPKPcmd.length
		for track in 0 ..< tmpPPKPcmd.tracks {
			for var row: Int16 = 0; row < pcmdLength; row += step {
				var myCmd = MADGetCmd(row, track, thePcmd);
				
				if pcmdLength > 1 {			// no zero div !!
					switch fadeType {
					case .Instrument:
						myCmd.memory.ins	= UInt8(from + ((to - from) * Int32(row)) / (Int32(pcmdLength) - 1))
						
					case .Note:
						myCmd.memory.note	= UInt8(from + ((to - from) * Int32(row)) / (Int32(pcmdLength) - 1))
						
					case .Argument:
						myCmd.memory.arg	= UInt8(from + ((to - from) * Int32(row)) / (Int32(pcmdLength) - 1))
						
					case .Volume:
						myCmd.memory.vol	= UInt8(from + ((to - from) * Int32(row)) / (Int32(pcmdLength) - 1))
					}
				}
			}
		}
		
		parentWindow.endSheet(window!)
		currentBlock(.NoErr)
	}
	
	@IBAction func cancel(sender: AnyObject!) {
		parentWindow.endSheet(window!)
		currentBlock(.UserCanceledErr)
	}
}
