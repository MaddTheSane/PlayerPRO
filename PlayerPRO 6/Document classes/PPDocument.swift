//
//  PPDocument.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/21/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit
import AVFoundation
import AudioToolbox

@objc(PPDocument) class PPDocument: NSDocument {	
	var instrumentList: InstrumentPanelController! = nil
	var mainViewController: DocumentWindowController! = nil
	dynamic let theDriver: PPDriver
	dynamic private(set) var theMusic: PPMusicObject!
	
	dynamic var musicName: String {
		get {
			return theMusic.title
		}
		set {
			theMusic.title = newValue
		}
	}
	
	dynamic var musicInfo: String {
		get {
			return theMusic.information
		}
		set {
			theMusic.information = newValue
		}
	}
	
	override func makeWindowControllers() {
		let docWinCon = DocumentWindowController(windowNibName: "PPDocument")
		addWindowController(docWinCon)
		docWinCon.currentDocument = self
		instrumentList = InstrumentPanelController(windowNibName: "InsPanel")
		addWindowController(instrumentList)
		instrumentList.currentDocument = self
		mainViewController = docWinCon
	}

	private func resetPlayerPRODriver() {
		var theSett = MADDriverSettings.new()
		let defaults = NSUserDefaults.standardUserDefaults()
		
		//TODO: Sanity Checking
		theSett.surround = defaults.boolForKey(PPSurroundToggle)
		theSett.outPutRate = UInt32(defaults.integerForKey(PPSoundOutRate))
		theSett.outPutBits = Int16(defaults.integerForKey(PPSoundOutBits))
		if (defaults.boolForKey(PPOversamplingToggle)) {
			theSett.oversampling = Int32(defaults.integerForKey(PPOversamplingAmount))
		} else {
			theSett.oversampling = 1;
		}
		theSett.Reverb = defaults.boolForKey(PPReverbToggle)
		theSett.ReverbSize = Int32(defaults.integerForKey(PPReverbAmount))
		theSett.ReverbStrength = Int32(defaults.integerForKey(PPReverbStrength))
		if (defaults.boolForKey(PPStereoDelayToggle)) {
			theSett.MicroDelaySize = Int32(defaults.integerForKey(PPStereoDelayAmount))
		} else {
			theSett.MicroDelaySize = 0;
		}
		
		theSett.driverMode = MADSoundOutput(rawValue: Int16(defaults.integerForKey(PPSoundDriver))) ?? .CoreAudioDriver
		theSett.repeatMusic = false;
		
		do {
			try theDriver.changeDriverSettingsToSettings(&theSett)
		} catch let error as NSError {
			print("Unable to change driver for \(self), error \(error)")
			//NSAlert(error: createErrorFromMADErrorType(returnerr)).beginSheetModalForWindow(self.windowForSheet, completionHandler: { (returnCode) -> Void in
			//currently, do nothing
			//})

		}
	}
	
	@objc private func soundPreferencesDidChange(notification: NSNotification) {
		resetPlayerPRODriver()
	}
	
	convenience init(music: PPMusicObject) {
		self.init()
		theMusic = music
	}
	
	override init() {
		var drivSettings = MADDriverSettings.new()
		let defaults = NSUserDefaults.standardUserDefaults()
		
		//TODO: Sanity Checking
		drivSettings.surround = defaults.boolForKey(PPSurroundToggle)
		drivSettings.outPutRate = UInt32(defaults.integerForKey(PPSoundOutRate))
		drivSettings.outPutBits = Int16(defaults.integerForKey(PPSoundOutBits))
		if defaults.boolForKey(PPOversamplingToggle) {
			drivSettings.oversampling = Int32(defaults.integerForKey(PPOversamplingAmount))
		} else {
			drivSettings.oversampling = 1
		}
		drivSettings.Reverb = defaults.boolForKey(PPReverbToggle)
		drivSettings.ReverbSize = Int32(defaults.integerForKey(PPReverbAmount))
		drivSettings.ReverbStrength = Int32(defaults.integerForKey(PPReverbStrength))
		if (defaults.boolForKey(PPStereoDelayToggle)) {
			drivSettings.MicroDelaySize = Int32(defaults.integerForKey(PPStereoDelayAmount))
		} else {
			drivSettings.MicroDelaySize = 0;
		}
		
		drivSettings.driverMode = MADSoundOutput(rawValue: Int16(defaults.integerForKey(PPSoundDriver))) ?? .CoreAudioDriver
		drivSettings.repeatMusic = false;
		
		theDriver = try! PPDriver(library: globalMadLib, settings: &drivSettings)
		super.init()
		
		let defaultCenter = NSNotificationCenter.defaultCenter()
		defaultCenter.addObserver(self, selector: #selector(PPDocument.soundPreferencesDidChange(_:)), name: PPSoundPreferencesDidChange, object: nil)
	}
	
    override func windowControllerDidLoadNib(aController: NSWindowController) {
        super.windowControllerDidLoadNib(aController)
        // Add any code here that needs to be executed once the windowController has loaded the document's window.
    }

	override func writeToURL(url: NSURL, ofType typeName: String) throws {
		if typeName != MADNativeUTI {
			throw NSError(domain: NSOSStatusErrorDomain, code: paramErr, userInfo: nil)
		} else {
			do {
				try theMusic.saveMusicToURL(url, compress: NSUserDefaults.standardUserDefaults().boolForKey(PPMMadCompression))
			} catch {
				if let error = error as? MADErr {
					throw error.convertToCocoaType()
				} else if (error as NSError).domain == PPMADErrorDomain {
					let bErr = MADErr(rawValue: Int16((error as NSError).code))!
					throw bErr.convertToCocoaType()
				} else {
					throw error
				}
			}
		}
	}
	
	override func readFromURL(url: NSURL, ofType typeName: String) throws {
		if typeName != MADNativeUTI {
			throw NSError(domain: NSOSStatusErrorDomain, code: paramErr, userInfo: nil)
		}
		
		theMusic = try PPMusicObject(URL: url, driver: theDriver)
	}
	
    override class func autosavesInPlace() -> Bool {
        return true
    }

	func importMusicObject(theObj: PPMusicObject) {
		if (theMusic == nil) {
			theMusic = theObj
		}
	}
		
	deinit {
		NSNotificationCenter.defaultCenter().removeObserver(self)
	}
}
