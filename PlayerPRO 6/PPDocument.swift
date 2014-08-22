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

private func generateAVMetadataInfo(oldMusicName: String, oldMusicInfo: String) -> [AVMutableMetadataItem] {
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
	@IBOutlet var exportWindow: NSWindow!
	@IBOutlet var exportSettingsBox: NSBox!
	@IBOutlet var fastForwardButton: NSButton!
	@IBOutlet var playButton: NSButton!
	@IBOutlet var reverseButton: NSButton!
	@IBOutlet var currentTimeLabel: NSTextField!
	@IBOutlet var totalTimeLabel: NSTextField!
	@IBOutlet var playbackPositionSlider: NSSlider!
	@IBOutlet var editorsTab: NSTabView!
	
	@IBOutlet var boxController: BoxViewController!
	@IBOutlet var digitalController: DigitalViewController!
	@IBOutlet var classicalController: ClassicalViewController!
	@IBOutlet var waveController: WaveViewController!
	
	var exportController = SoundSettingsViewController()
	private(set) var theDriver: PPDriver!
	private(set) var theMusic: PPMusicObject!
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
	private var exportSettings = MADDriverSettings()

    override var windowNibName: String {
        // Override returning the nib file name of the document
        // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
        return "PPDocument"
    }

	override init() {
		var drivSettings = MADDriverSettings()
		
			drivSettings.resetToBestDriver()
			var defaults = NSUserDefaults.standardUserDefaults()
			
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
        outError.memory = NSError.errorWithDomain(NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
        return nil
    }

	override func readFromURL(url: NSURL!, ofType typeName: String!, error outError: NSErrorPointer) -> Bool {
		outError.memory = NSError.errorWithDomain(NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
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
	
	func rawSoundData(inout theSet: MADDriverSettings) -> NSMutableData? {
		var err = MADErr.NoErr;
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
		
		var soundPtr:UnsafeMutablePointer<()> = nil;
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
		soundPtr = UnsafeMutablePointer<()>.alloc(full)
		
		while theRec.directSaveToPointer(soundPtr, settings: &theSet) {
			mutData.appendBytes(soundPtr, length: full)
		}
		
		soundPtr.dealloc(full)
		
		return mutData;
	}
	
	func rawBESoundData(inout theSet: MADDriverSettings) -> NSData? {
		let rsd = rawSoundData(&theSet)
		if rsd == nil {
			return nil
		}
		if (theSet.outPutBits == 16) {
			let sndSize = rsd!.length;
			let bePtr = UnsafeMutablePointer<UInt16>(rsd!.mutableBytes)
			dispatch_apply(UInt(sndSize) / 2, dispatch_get_global_queue(0, 0), { (i) -> Void in
				bePtr[Int(i)] = bePtr[Int(i)].bigEndian
				return
			})
		}
		return rsd!.copy() as NSData?
	}
	
	func rawLESoundData(inout theSet: MADDriverSettings) -> NSData? {
		return rawSoundData(&theSet)?.copy() as NSData?
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
