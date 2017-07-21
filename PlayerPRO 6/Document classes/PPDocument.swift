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
	@objc dynamic let theDriver: PPDriver
	@objc dynamic private(set) var theMusic: PPMusicObject!
	
	@objc dynamic var musicName: String {
		get {
			return theMusic.title
		}
		set {
			theMusic.title = newValue
		}
	}
	
	@objc dynamic var musicInfo: String {
		get {
			return theMusic.information
		}
		set {
			theMusic.information = newValue
		}
	}
	
	// MARK: - NSDocument type info
	override class func isNativeType(_ aType: String) -> Bool {
		return aType == MADNativeUTI
	}
	
	override class var readableTypes: [String] {
		struct StaticStorage {
			static var readables: [String]?
		}
		
		if let readables = StaticStorage.readables {
			return readables
		}
		
		let importUTIsArray = globalMadLib.filter({ $0.canImport == true }).map({ $0.UTITypes })
		let importUTISet: Set<String> = {
			var toRet = Set<String>()
			for arr in importUTIsArray {
				toRet.formUnion(arr)
			}
			return toRet
		}()
		var toRet = Array(importUTISet)
		toRet.insert(MADNativeUTI, at: 0)
		toRet.append(MADGenericUTI)
		StaticStorage.readables = toRet
		return toRet
	}
	
	override class var writableTypes: [String] {
		struct StaticStorage {
			static var writables: [String]?
		}
		
		if let writables = StaticStorage.writables {
			return writables
		}
		
		var toRet = globalMadLib.filter({ $0.canExport == true }).map({ $0.UTITypes.first! })
		toRet.insert(MADNativeUTI, at: 0)
		StaticStorage.writables = toRet
		return toRet
	}
	
	// MARK: -
	
	override func makeWindowControllers() {
		let docWinCon = DocumentWindowController(windowNibName: NSNib.Name(rawValue: "PPDocument"))
		addWindowController(docWinCon)
		docWinCon.currentDocument = self
		instrumentList = InstrumentPanelController(windowNibName: NSNib.Name(rawValue: "InsPanel"))
		addWindowController(instrumentList)
		instrumentList.currentDocument = self
		mainViewController = docWinCon
	}

	private func resetPlayerPRODriver() {
		var theSett = MADDriverSettings.new()
		let defaults = UserDefaults.standard
		
		//TODO: Sanity Checking
		theSett.surround = defaults.bool(forKey: PPSurroundToggle)
		theSett.outPutRate = UInt32(defaults.integer(forKey: PPSoundOutRate))
		theSett.outPutBits = Int16(defaults.integer(forKey: PPSoundOutBits))
		if (defaults.bool(forKey: PPOversamplingToggle)) {
			theSett.oversampling = Int32(defaults.integer(forKey: PPOversamplingAmount))
		} else {
			theSett.oversampling = 1;
		}
		theSett.Reverb = defaults.bool(forKey: PPReverbToggle)
		theSett.ReverbSize = Int32(defaults.integer(forKey: PPReverbAmount))
		theSett.ReverbStrength = Int32(defaults.integer(forKey: PPReverbStrength))
		if (defaults.bool(forKey: PPStereoDelayToggle)) {
			theSett.MicroDelaySize = Int32(defaults.integer(forKey: PPStereoDelayAmount))
		} else {
			theSett.MicroDelaySize = 0;
		}
		
		theSett.driverMode = MADSoundOutput(rawValue: Int16(defaults.integer(forKey: PPSoundDriver))) ?? .CoreAudioDriver
		theSett.repeatMusic = false;
		
		do {
			try theDriver.changeDriverSettings(to: &theSett)
		} catch {
			Swift.print("Unable to change driver for \(self), error '\(error)'")
			//NSAlert(error: createErrorFromMADErrorType(returnerr)).beginSheetModalForWindow(self.windowForSheet, completionHandler: { (returnCode) -> Void in
			//currently, do nothing
			//})

		}
	}
	
	@objc private func soundPreferencesDidChange(_ notification: Notification) {
		resetPlayerPRODriver()
	}
	
	convenience init(music: PPMusicObject) {
		self.init()
		theMusic = music
	}
	
	override init() {
		var drivSettings = MADDriverSettings.new()
		let defaults = UserDefaults.standard
		
		//TODO: Sanity Checking
		drivSettings.surround = defaults.bool(forKey: PPSurroundToggle)
		drivSettings.outPutRate = UInt32(defaults.integer(forKey: PPSoundOutRate))
		drivSettings.outPutBits = Int16(defaults.integer(forKey: PPSoundOutBits))
		if defaults.bool(forKey: PPOversamplingToggle) {
			drivSettings.oversampling = Int32(defaults.integer(forKey: PPOversamplingAmount))
		} else {
			drivSettings.oversampling = 1
		}
		drivSettings.Reverb = defaults.bool(forKey: PPReverbToggle)
		drivSettings.ReverbSize = Int32(defaults.integer(forKey: PPReverbAmount))
		drivSettings.ReverbStrength = Int32(defaults.integer(forKey: PPReverbStrength))
		if (defaults.bool(forKey: PPStereoDelayToggle)) {
			drivSettings.MicroDelaySize = Int32(defaults.integer(forKey: PPStereoDelayAmount))
		} else {
			drivSettings.MicroDelaySize = 0;
		}
		
		drivSettings.driverMode = MADSoundOutput(rawValue: Int16(defaults.integer(forKey: PPSoundDriver))) ?? .CoreAudioDriver
		drivSettings.repeatMusic = false;
		
		theDriver = try! PPDriver(library: globalMadLib, settings: &drivSettings)
		super.init()
		
		let defaultCenter = NotificationCenter.default
		defaultCenter.addObserver(self, selector: #selector(PPDocument.soundPreferencesDidChange(_:)), name: .PPSoundPreferencesDidChange, object: nil)
	}
	
    override func windowControllerDidLoadNib(_ aController: NSWindowController) {
        super.windowControllerDidLoadNib(aController)
        // Add any code here that needs to be executed once the windowController has loaded the document's window.
    }

	override func write(to url: URL, ofType typeName: String) throws {
		if typeName != MADNativeUTI {
			guard let type = globalMadLib.typeFromUTI(typeName) else {
				throw NSError(domain: NSOSStatusErrorDomain, code: paramErr, userInfo: nil)
			}
			try theMusic.exportMusic(to: url, format: type, library: globalMadLib)
		} else {
			do {
				try theMusic.saveMusic(to: url, compress: UserDefaults.standard.bool(forKey: PPMMadCompression))
			} catch {
				if let error = error as? MADErr {
					throw error.convertToCocoaType()
				} else {
					throw error
				}
			}
		}
	}
	
	//override func write(to url: URL, ofType typeName: String, for saveOperation: NSSaveOperationType, originalContentsURL absoluteOriginalContentsURL: URL?) throws {
	//	try super.write(to: url, ofType: typeName, for: saveOperation, originalContentsURL: absoluteOriginalContentsURL)
	//}
	
	override func read(from url: URL, ofType typeName: String) throws {
		func getType() throws -> String {
			if let type = globalMadLib.typeFromUTI(typeName) {
				return type
			}
			
			return try globalMadLib.identifyFile(at: url)
		}
		
		if typeName == MADNativeUTI {
			theMusic = try PPMusicObject(url: url, driver: theDriver)
		} else {
			let theType = try getType()
			theMusic = try PPMusicObject(url: url, stringType: theType, driver: theDriver)
			fileURL = nil
		}
	}
	
	override var autosavingFileType: String? {
		return MADNativeUTI
	}
	
	override class var autosavesInPlace: Bool {
		return true
	}
	/*
    override class func autosavesInPlace() -> Bool {
        return true
    }*/

	func importMusicObject(_ theObj: PPMusicObject) {
		if theMusic == nil {
			theMusic = theObj
		}
	}
		
	deinit {
		NotificationCenter.default.removeObserver(self)
	}
}
