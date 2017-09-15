//
//  MIDIImporterController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/28/14.
//
//

import Cocoa

class MIDIImporterController: NSWindowController {
	@IBOutlet weak var theURLField: NSTextField!
	private var locationOfFile: URL!
	@objc dynamic var QTIns = false
	@objc dynamic var trackCount = 8
	private var handler: PPComplexImportHandler!
	private var modalSession: NSApplication.ModalSession!
	
	class func newWithLocation(_ loc: URL, handler hand: @escaping PPComplexImportHandler) -> MIDIImporterController {
		let impCont = MIDIImporterController(windowNibName: NSNib.Name(rawValue: "MIDIImporterController"))
		impCont.locationOfFile = loc
		impCont.handler = hand
		
		return impCont
	}
	
	func beginImportModalSession() {
		trackCount = GetTracksNumber(locationOfFile);
		if trackCount == -1 {
			handler(nil, MADErr.incompatibleFile)
			return
		}
		modalSession = NSApplication.shared.beginModalSession(for: window!)
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		theURLField.stringValue = locationOfFile.path
	}
	
	@IBAction func okayButtonPressed(sender: AnyObject) {
		NSApplication.shared.endModalSession(modalSession)
		let conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.MIDI-Import")
		conn.remoteObjectInterface = NSXPCInterface(with: PPMIDIImportHelper.self)
		
		conn.resume()
		let conn2 = conn.remoteObjectProxyWithErrorHandler { (err) in
			self.handler(nil, err)
		} as! PPMIDIImportHelper
		conn2.importMIDIFile(at: locationOfFile, numberOfTracks: trackCount, useQTInstruments: true, withReply:{ (aDat, aErr) -> Void in
			OperationQueue.main.addOperation {
				if aErr == MADErr.noErr {
					let tmpObj = MIDIReadFromData(aDat)
					self.handler(tmpObj, nil)
				} else {
					self.handler(nil, aErr)
				}
				conn.invalidate()
			}
		})
	}

	@IBAction func cancelButtonPressed(sender: AnyObject) {
		NSApplication.shared.endModalSession(modalSession)
		handler(nil, MADErr.userCanceledErr)
	}
}
