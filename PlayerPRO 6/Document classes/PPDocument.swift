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

private func generateAVMetadataInfo(oldMusicName: String, oldMusicInfo: String) -> [AVMetadataItem] {
	var titleName = AVMutableMetadataItem()
	titleName.keySpace = AVMetadataKeySpaceCommon
	titleName.setKey(AVMetadataCommonKeyTitle)
	titleName.setValue(oldMusicName)
	
	var dataInfo = AVMutableMetadataItem()
	dataInfo.keySpace = AVMetadataKeySpaceCommon;
	dataInfo.setKey(AVMetadataCommonKeySoftware)
	dataInfo.setValue("PlayerPRO 6")
	
	var musicInfoQTUser = AVMutableMetadataItem();
	musicInfoQTUser.keySpace = AVMetadataKeySpaceQuickTimeUserData
	musicInfoQTUser.setKey(AVMetadataQuickTimeUserDataKeyInformation)
	musicInfoQTUser.setValue(oldMusicInfo)
	
	var musicInfoiTunes = AVMutableMetadataItem();
	musicInfoiTunes.keySpace = AVMetadataKeySpaceiTunes
	musicInfoiTunes.setKey(AVMetadataiTunesMetadataKeyUserComment)
	musicInfoiTunes.setValue(oldMusicInfo)
	
	var musicInfoQTMeta = AVMutableMetadataItem();
	musicInfoQTMeta.keySpace = AVMetadataKeySpaceQuickTimeMetadata
	musicInfoQTMeta.setKey(AVMetadataQuickTimeMetadataKeyInformation)
	musicInfoQTMeta.setValue(oldMusicInfo)
	
	return [titleName, dataInfo, musicInfoQTUser, musicInfoiTunes, musicInfoQTMeta];
}

@objc(PPDocument) class PPDocument: NSDocument, SoundSettingsViewControllerDelegate {
	@IBOutlet weak var exportWindow:			NSWindow!
	@IBOutlet weak var exportSettingsBox:		NSBox!
	@IBOutlet weak var fastForwardButton:		NSButton!
	@IBOutlet weak var playButton:				NSButton!
	@IBOutlet weak var reverseButton:			NSButton!
	@IBOutlet weak var currentTimeLabel:		NSTextField!
	@IBOutlet weak var totalTimeLabel:			NSTextField!
	@IBOutlet weak var playbackPositionSlider:	NSSlider!
	@IBOutlet weak var editorsTab:				NSTabView!
	
	@IBOutlet weak var boxController:		BoxViewController!
	@IBOutlet weak var digitalController:	DigitalViewController!
	@IBOutlet weak var classicalController:	ClassicalViewController!
	@IBOutlet weak var waveController:		WaveViewController!
	
	var exportController = SoundSettingsViewController()
	dynamic let theDriver: PPDriver
	dynamic private(set) var theMusic: PPMusicObject!
	private var exportSettings = MADDriverSettings()
	dynamic var wrapper: PPMusicObject { get {
		return theMusic
		}}
	
	dynamic var musicName: String {
		get {
			return theMusic.internalFileName
		} set {
			theMusic.internalFileName = newValue
		}}
	
	dynamic var musicInfo: String {
		get {
			return theMusic.madInformation
		} set {
			theMusic.madInformation = newValue
		}}
	

    override var windowNibName: String {
        // Override returning the nib file name of the document
        // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
        return "PPDocument"
    }

	private func resetPlayerPRODriver() {
		var returnerr = MADErr.NoErr;
		var theSett = MADDriverSettings(bestDriver: true)
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
		
		theSett.driverMode = MADSoundOutput.fromRaw(Int16(defaults.integerForKey(PPSoundDriver)))!
		theSett.repeatMusic = false;
		
		returnerr = theDriver.changeDriverSettingsToSettings(theSett)
		
		if (returnerr != MADErr.NoErr) {
			println("Unable to change driver, \(self)")
			//NSAlert(error: CreateErrorFromMADErrorType(returnerr)).beginSheetModalForWindow(self.windowForSheet, completionHandler: { (returnCode) -> Void in
				 //currently, do nothing
			//})
		}
	}
	
	private func soundPreferencesDidChange(notification: NSNotification) {
		resetPlayerPRODriver()
	}
	
	convenience init(music: PPMusicObject) {
		self.init()
		theMusic = music
	}
	
	override init() {
		var drivSettings = MADDriverSettings(bestDriver: true)
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
		
		drivSettings.driverMode = MADSoundOutput.fromRaw(Int16(defaults.integerForKey(PPSoundDriver)))!
		drivSettings.repeatMusic = false;
		
		theDriver = PPDriver(library: globalMadLib, settings: &drivSettings)
		super.init()
		exportController.delegate = self;
		
		let defaultCenter = NSNotificationCenter.defaultCenter()
		defaultCenter.addObserver(self, selector: "soundPreferencesDidChange:", name: PPSoundPreferencesDidChange, object: nil)
	}
	
    override func windowControllerDidLoadNib(aController: NSWindowController?) {
        super.windowControllerDidLoadNib(aController)
        // Add any code here that needs to be executed once the windowController has loaded the document's window.
    }

    override func dataOfType(typeName: String?, error outError: NSErrorPointer) -> NSData? {
        // Insert code here to write your document to data of the specified type. If outError != NULL, ensure that you create and set an appropriate error when returning nil.
        // You can also choose to override -fileWrapperOfType:error:, -writeToURL:ofType:error:, or -writeToURL:ofType:forSaveOperation:originalContentsURL:error: instead.
		outError.memory = NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
        return nil
    }

	override func readFromURL(url: NSURL!, ofType typeName: String!, error outError: NSErrorPointer) -> Bool {
		outError.memory = NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
		return false
	}
	
    override class func autosavesInPlace() -> Bool {
        return true
    }

	func importMusicObject(theObj: PPMusicObject) {
		if (theMusic == nil) {
			self.theMusic = theObj;
		}
	}
	
	func rawSoundData(theSet1: MADDriverSettings) -> NSMutableData? {
		var err = MADErr.NoErr;
		var theSet = theSet1
		var theRec = PPDriver(library: globalMadLib, settings: &theSet, error: &err)
		
		if (err != MADErr.NoErr) {
			dispatch_async(dispatch_get_main_queue()) {
				let NSerr = CreateErrorFromMADErrorType(err)!
				NSAlert(error: NSerr).beginSheetModalForWindow(self.windowForSheet, completionHandler: { (returnCode) -> Void in
					//Do nothing
				})
			}
			
			return nil;
		}
		
		theRec.cleanDriver()
		theRec.currentMusic = theMusic
		
		var soundPtr: UnsafeMutablePointer<UInt8> = nil;
		var full = theRec.audioDataLength
		if (theSet.outPutBits == 16) {
			full *= 2;
		} else if (theSet.outPutBits == 20 || theSet.outPutBits == 24 ) {
			full *= 3;
		}
		
		switch (theSet.outPutMode) {
		case .DeluxeStereoOutPut:
			fallthrough
		case .StereoOutPut:
			full *= 2;
			break;
			
		case .PolyPhonic:
			full *= 4;
			break;
			
		default:
			break;
		}
		
		theRec.play()
		
		var mutData = NSMutableData(capacity: full * 60 * Int(theRec.totalMusicPlaybackTime) / 2)
		soundPtr = UnsafeMutablePointer<UInt8>.alloc(full)
		
		while theRec.directSaveToPointer(soundPtr, settings: &theSet) {
			mutData.appendBytes(soundPtr, length: full)
		}
		
		soundPtr.dealloc(full)
		
		return mutData;
	}
	
	func rawBESoundData(theSet: MADDriverSettings) -> NSData? {
		let rsd = rawSoundData(theSet)
		if rsd == nil {
			return nil
		}
		if (theSet.outPutBits == 16) {
			let sndSize = rsd!.length;
			let bePtr = UnsafeMutablePointer<UInt16>(rsd!.mutableBytes)
			dispatch_apply(UInt(sndSize) / 2, dispatch_get_global_queue(0, 0), { (i) -> Void in
				let iInt = Int(i)
				bePtr[iInt] = bePtr[iInt].bigEndian
				return
			})
		}
		return rsd?.copy() as NSData?
	}
	
	func rawLESoundData(theSet: MADDriverSettings) -> NSData? {
		return rawSoundData(theSet)?.copy() as NSData?
	}

	private func applyMetadataToFileID(theID: AudioFileID) {
		//TODO: implement, but how?
	}
	
	private func saveMusic(waveToURL theURL: NSURL, theSett: MADDriverSettings) -> MADErr {
		let saveData = rawLESoundData(theSett)
		var audioFile: AudioFileID = nil;
		if saveData == nil {
			return MADErr.NeedMemory
		}
		var tmpBytes: UInt32 = 0
		switch (theSett.outPutMode) {
		case .MonoOutPut:
			tmpBytes = 1;
			break;
			
			
		case .PolyPhonic:
			tmpBytes = 4;
			break;
			
		default:
			tmpBytes = 2;
			break;
		}

		var tmpabc: UInt32 = UInt32(theSett.outPutBits) * tmpBytes / 8
		
		var asbd = AudioStreamBasicDescription(mSampleRate: Float64(theSett.outPutRate), mFormatID: UInt32(kAudioFormatLinearPCM), mFormatFlags: UInt32(kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked), mBytesPerPacket: tmpabc, mFramesPerPacket: 1, mBytesPerFrame: tmpabc, mChannelsPerFrame: tmpBytes, mBitsPerChannel: UInt32(theSett.outPutBits), mReserved: 0)
		var res = AudioFileCreateWithURL(theURL, UInt32(kAudioFileWAVEType), &asbd, UInt32(kAudioFileFlags_EraseFile), &audioFile);
		if (res != noErr) {
			if (audioFile != nil) {
				AudioFileClose(audioFile);
			}
			return MADErr.WritingErr;
		}

		var numBytes = UInt32(saveData!.length)
		res = AudioFileWriteBytes(audioFile, 0, 0, &numBytes, saveData!.bytes);
		if (res != noErr) {
			if (audioFile != nil) {
				AudioFileClose(audioFile);
			}
			return MADErr.WritingErr;
		}

		applyMetadataToFileID(audioFile)
		res = AudioFileClose(audioFile);
		if (res != noErr) {
			if (audioFile != nil) {
				AudioFileClose(audioFile);
			}
			return MADErr.WritingErr;
		}
		
		return MADErr.NoErr
	}
	
	private func saveMusic(AIFFToURL theURL: NSURL, theSett: MADDriverSettings) -> MADErr {
		let saveData = rawBESoundData(theSett)
		var audioFile: AudioFileID = nil;
		if saveData == nil {
			return MADErr.NeedMemory
		}
		var tmpBytes: UInt32 = 0
		switch (theSett.outPutMode) {
		case .MonoOutPut:
			tmpBytes = 1;
			break;
			
		case .PolyPhonic:
			tmpBytes = 4;
			break;
			
		default:
			tmpBytes = 2;
			break;
		}

		var tmpabc: UInt32 = UInt32(theSett.outPutBits) * tmpBytes / 8
		
		var asbd = AudioStreamBasicDescription(mSampleRate: Float64(theSett.outPutRate), mFormatID: UInt32(kAudioFormatLinearPCM), mFormatFlags: UInt32(kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked | kAudioFormatFlagIsBigEndian), mBytesPerPacket: tmpabc, mFramesPerPacket: 1, mBytesPerFrame: tmpabc, mChannelsPerFrame: tmpBytes, mBitsPerChannel: UInt32(theSett.outPutBits), mReserved: 0)
		var res = AudioFileCreateWithURL(theURL, UInt32(kAudioFileAIFFType), &asbd, UInt32(kAudioFileFlags_EraseFile), &audioFile);
		if (res != noErr) {
			if (audioFile != nil) {
				AudioFileClose(audioFile);
			}
			return MADErr.WritingErr;
		}

		var numBytes = UInt32(saveData!.length)
		res = AudioFileWriteBytes(audioFile, 0, 0, &numBytes, saveData!.bytes);
		if (res != noErr) {
			if (audioFile != nil) {
				AudioFileClose(audioFile);
			}
			return MADErr.WritingErr;
		}

		applyMetadataToFileID(audioFile)
		res = AudioFileClose(audioFile);
		if (res != noErr) {
			if (audioFile != nil) {
				AudioFileClose(audioFile);
			}
			return MADErr.WritingErr;
		}
		
		return MADErr.NoErr
	}
	
	private func showExportSettingsWithExportType(expType: Int, URL theURL: NSURL) {
		//MADGetBestDriver(&exportSettings);
		exportSettings.driverMode = .NoHardwareDriver;
		exportSettings.repeatMusic = false;
		exportController.settingsFromDriverSettings(exportSettings)
		
		self.windowForSheet.beginSheet(exportWindow, completionHandler: { (returnCode) -> Void in
			if (returnCode == NSAlertDefaultReturn) {
				switch (expType) {
				case -1:
					let expObj = ExportObject(destination: theURL, exportBlock: { (theURL, errStr) -> MADErr in
						if errStr != nil {
							errStr.memory = nil
						}
						var theErr = self.saveMusic(AIFFToURL: theURL, theSett: self.exportSettings)
						self.theDriver.endExport()
						return theErr
					})
					(NSApplication.sharedApplication().delegate as AppDelegate).addExportObject(expObj)
					
				case -2:
					let expObj = ExportObject(destination: theURL, exportBlock: { (theURL, errStr) -> MADErr in
						var theErr = MADErr.NoErr;
						if errStr != nil {
							errStr.memory = nil
						}
						
						let oldMusicName = self.musicName;
						let oldMusicInfo = self.musicInfo;
						let oldURL = self.fileURL;
						let tmpURL = NSFileManager.defaultManager().URLForDirectory(.ItemReplacementDirectory, inDomain:.UserDomainMask, appropriateForURL:oldURL, create:true, error:nil)!.URLByAppendingPathComponent( NSString(format:"%@.aiff", oldMusicName != "" ? oldMusicName : "untitled"), isDirectory: false)
						theErr = self.saveMusic(AIFFToURL: tmpURL, theSett: self.exportSettings)
						self.theDriver.endExport()
						if (theErr != MADErr.NoErr) {
							if (errStr != nil) {
								errStr.memory = "Unable to save temporary file to \(tmpURL.path), error \(theErr)."
							}
							
							return theErr;
						}
						
						var exportMov = AVAsset.assetWithURL(tmpURL) as AVAsset?
						let metadataInfo = generateAVMetadataInfo(oldMusicName, oldMusicInfo)
						
						if (exportMov == nil) {
							if (errStr != nil) {
								errStr.memory = "Init failed for \(oldMusicName)"
							}
							NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
							return .WritingErr;
						}
						
						var tmpsession = AVAssetExportSession(asset: exportMov, presetName: AVAssetExportPresetAppleM4A) as AVAssetExportSession?
						if (tmpsession == nil) {
							if (errStr != nil) {
								errStr.memory = "Export session creation for \(oldMusicName) failed."
							}
							NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
							return .WritingErr;
						}
						let session = tmpsession!
						NSFileManager.defaultManager().removeItemAtURL(theURL, error: nil)
						session.outputURL = theURL;
						session.outputFileType = AVFileTypeAppleM4A;
						session.metadata = metadataInfo;
						var sessionWaitSemaphore = dispatch_semaphore_create(0);
						session.exportAsynchronouslyWithCompletionHandler({ () -> Void in
							dispatch_semaphore_signal(sessionWaitSemaphore);
							return;
						})
						dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER);
						
						var didFinish = session.status == .Completed;
						NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
						
						if (didFinish) {
							return .NoErr;
						} else {
							if (errStr != nil) {
								errStr.memory = "export of \"\(oldMusicName)\" failed."
							}
							return .WritingErr;
						}
					})
					(NSApplication.sharedApplication().delegate as AppDelegate).addExportObject(expObj)
					
				default:
					self.theDriver.exporting = false
				}
			}
		})
	}
	
	@IBAction func exportMusicAs(sender: AnyObject!) {
		let tag = (sender as NSMenuItem).tag
		theDriver.beginExport()
		let savePanel = NSSavePanel()
		let musicName = self.musicName;
		savePanel.prompt = "Export"
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		
		if musicName != "" {
			savePanel.nameFieldStringValue = musicName
		}
		
		switch (tag) {
		case -1:
			//AIFF
			savePanel.allowedFileTypes = [AVFileTypeAIFF]
			savePanel.title = "Export as AIFF audio"
			
			savePanel.beginSheetModalForWindow(self.windowForSheet, completionHandler: { (result) -> Void in
				if result == NSFileHandlingPanelOKButton {
					self.showExportSettingsWithExportType(-1, URL: savePanel.URL)
				} else {
					self.theDriver.endExport()
				}
			})
			
			break;
			
		case -2:
			//MP4
			savePanel.allowedFileTypes = [AVFileTypeAppleM4A]
			savePanel.title = "Export as MPEG-4 Audio"
			savePanel.beginSheetModalForWindow(self.windowForSheet, completionHandler: { (result) -> Void in
				if result == NSFileHandlingPanelOKButton {
					self.showExportSettingsWithExportType(-2, URL: savePanel.URL)
				} else {
					self.theDriver.endExport()
				}
			})
			break;
			
		default:
			
			if (tag > Int(globalMadLib.pluginCount) || tag < 0) {
				NSBeep();
				self.theDriver.endExport()
				
				return;
			}
			let tmpObj = globalMadLib[tag];
			
			savePanel.allowedFileTypes = tmpObj.UTItypes
			savePanel.title = "Export as \(tmpObj.menuName)"
			
			savePanel.beginSheetModalForWindow(self.windowForSheet, completionHandler: { (result) -> Void in
				if result == NSFileHandlingPanelOKButton {
					var expObj = ExportObject(destination: savePanel.URL, exportBlock: { (theURL, errStr) -> MADErr in
						var theErr = self.theMusic.exportMusicToURL(theURL, format: tmpObj.plugType, library: globalMadLib)
						self.theDriver.endExport()
						return theErr
					})
				} else {
					self.theDriver.exporting = false
				}
			})
			break;
		}
	}
	
	@IBAction func showBoxEditor(sender: AnyObject!) {
		editorsTab.selectTabViewItemWithIdentifier("Box")
	}
	
	@IBAction func showClassicEditor(sender: AnyObject!) {
		editorsTab.selectTabViewItemWithIdentifier("Classic")
	}
	
	@IBAction func showDigitalEditor(sender: AnyObject!) {
		editorsTab.selectTabViewItemWithIdentifier("Digital")
	}
	
	@IBAction func showWavePreview(sender: AnyObject!) {
		editorsTab.selectTabViewItemWithIdentifier("Wave")
	}
	
	@IBAction func okayExportSettings(sender: AnyObject!) {
		NSApplication.sharedApplication().endSheet(exportWindow, returnCode: NSAlertDefaultReturn)
		exportWindow.close()
	}
	
	@IBAction func cancelExportSettings(sender: AnyObject!) {
		NSApplication.sharedApplication().endSheet(exportWindow, returnCode: NSAlertAlternateReturn)
		exportWindow.close()
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		exportSettingsBox.contentView = exportController.view
	}
	
	deinit {
		NSNotificationCenter.defaultCenter().removeObserver(self)
	}
	
	func soundOutBitsDidChange(bits: Int16) {
		exportSettings.outPutBits = bits;
	}
	
	func soundOutRateDidChange(rat: UInt32) {
		exportSettings.outPutRate = rat;
	}
	
	func soundOutReverbDidChangeActive(isAct: Bool) {
		exportSettings.Reverb = isAct;
	}
	
	func soundOutOversamplingDidChangeActive(isAct: Bool) {
		if (!isAct) {
			exportSettings.oversampling = 1;
		}
	}
	
	func soundOutStereoDelayDidChangeActive(isAct: Bool) {
		if (!isAct) {
			exportSettings.MicroDelaySize = 0;
		}
	}
	
	func soundOutSurroundDidChangeActive(isAct: Bool) {
		exportSettings.surround = isAct;
	}
	
	func soundOutReverbStrengthDidChange(rev: Int32) {
		exportSettings.ReverbStrength = rev;
	}
	
	func soundOutReverbSizeDidChange(rev: Int32) {
		exportSettings.ReverbSize = rev;
	}
	
	func soundOutOversamplingAmountDidChange(ovs: Int32) {
		exportSettings.oversampling = ovs;
	}
	
	func soundOutStereoDelayAmountDidChange(std: Int32) {
		exportSettings.MicroDelaySize = std;
	}
}
