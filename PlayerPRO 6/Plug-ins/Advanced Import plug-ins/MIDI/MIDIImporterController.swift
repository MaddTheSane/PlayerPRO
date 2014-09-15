//
//  MIDIImporterController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/28/14.
//
//

import Cocoa

/*final*/ class MIDIImporterController: NSWindowController {
	@IBOutlet weak var theURLField: NSTextField!
	private var locationOfFile: NSURL!
	dynamic var QTIns = false
	dynamic var trackCount = 8
	private var handler: PPComplexImportHandler!
	private var modalSession: NSModalSession!
	
	class func newWithLocation(loc: NSURL, handler hand: PPComplexImportHandler) -> MIDIImporterController {
		let impCont = MIDIImporterController()
		impCont.locationOfFile = loc
		impCont.handler = hand
		
		return impCont
	}
	
	func beginImportModalSession() {
		trackCount = GetTracksNumber(locationOfFile);
		if trackCount == -1 {
			//NSApplication.sharedApplication().endModalSession(modalSession)
			handler(nil, .IncompatibleFile)
			return
		}
		modalSession = NSApplication.sharedApplication().beginModalSessionForWindow(self.window)
		return
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		theURLField.stringValue = locationOfFile.path!
	}
	
	@IBAction func okayButtonPressed(sender: AnyObject) {
		let conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.MIDI-Import")
		conn.remoteObjectInterface = NSXPCInterface(`protocol`: PPMIDIImportHelper.self)
		
		conn.resume()
		conn.remoteObjectProxy.importMIDIFileAtURL(locationOfFile, numberOfTracks: trackCount, useQTInstruments: true, withReply:{ (aDat, aErr) -> Void in
			if aErr == MADErr.NoErr {
				let tmpObj = MIDIReadFromData(aDat)
				self.handler(tmpObj, aErr)
			} else {
				self.handler(nil, aErr)
			}
			conn.invalidate();
		})
		NSApplication.sharedApplication().endModalSession(modalSession)
	}

	@IBAction func cancelButtonPressed(sender: AnyObject) {
		handler(nil, .UserCanceledErr)
		NSApplication.sharedApplication().endModalSession(modalSession)
	}
}
