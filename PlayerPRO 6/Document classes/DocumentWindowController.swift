//
//  DocumentWindowController.swift
//  PPMacho
//
//  Created by C.W. Betts on 10/11/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit
import AVFoundation
import AudioToolbox
import SwiftAdditions

extension MADDriverSettings {
	private init() {
		numChn			= 4
		outPutBits		= 16
		outPutMode		= .DeluxeStereoOutPut
		outPutRate		= 44100
		MicroDelaySize	= 25
		ReverbSize		= 100
		ReverbStrength	= 20
		oversampling	= 1
		TickRemover		= false
		surround		= false
		Reverb			= false
		repeatMusic		= true
		//reserved = 0
		//Just going to use CoreAudio
		driverMode		= .CoreAudioDriver;
	}
}

class DocumentWindowController: NSWindowController, SoundSettingsViewControllerDelegate {
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
	
	@IBOutlet weak var infoWindow:		NSWindow!
	@IBOutlet weak var infoNameField:	NSTextField!
	@IBOutlet weak var infoInfoField:	NSTextField!
	
	weak var currentDocument: PPDocument!
	private var exportSettings = MADDriverSettings()

	let exportController = SoundSettingsViewController.newSoundSettingWindow()!

	override func awakeFromNib() {
		super.awakeFromNib()
		exportController.delegate = self;
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		exportSettingsBox.contentView = exportController.view

	}
	
	override var windowNibName: String {
		// Override returning the nib file name of the document
		// If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
		return "PPDocument"
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
		currentDocument.windowForSheet!.endSheet(exportWindow, returnCode: NSAlertDefaultReturn)
		exportWindow.close()
	}
	
	@IBAction func cancelExportSettings(sender: AnyObject!) {
		currentDocument.windowForSheet!.endSheet(exportWindow, returnCode: NSAlertAlternateReturn)
		exportWindow.close()
	}
	
	@IBAction func showMusicInfo(sender: AnyObject?) {
		infoInfoField.stringValue = currentDocument.musicInfo
		infoNameField.stringValue = currentDocument.musicName
		
		currentDocument.windowForSheet!.beginSheet(infoWindow, completionHandler: { (response) -> Void in
			switch response {
			case NSAlertDefaultReturn:
				let um = self.currentDocument.undoManager!
				um.beginUndoGrouping()
				
				//let aDoc: (DocumentWindowController) = um.prepareWithInvocationTarget(self) as (DocumentWindowController)
				
				if self.currentDocument.musicInfo != self.infoInfoField.stringValue {
					um.registerUndoWithTarget(self.currentDocument, selector: "setMusicInfo:", object: self.currentDocument.musicInfo)
					//aDoc.musicInfo = self.currentDocument.musicInfo
					self.currentDocument.musicInfo = self.infoInfoField.stringValue
				}
				
				if self.currentDocument.musicName != self.infoNameField.stringValue {
					um.registerUndoWithTarget(self.currentDocument, selector: "setMusicName:", object: self.currentDocument.musicName)

					//aDoc.musicName = self.currentDocument.musicName
					self.currentDocument.musicName = self.infoNameField.stringValue
				}
				
				um.endUndoGrouping()
				
				um.setActionName("Info Change")
				
			default:
				break;
			}
			return
		})
	}
	
	@IBAction func okayMusicInfo(sender: AnyObject?) {
		currentDocument.windowForSheet!.endSheet(infoWindow, returnCode: NSAlertDefaultReturn)
		infoWindow.close()
	}
	
	@IBAction func cancelMusicInfo(sender: AnyObject?) {
		currentDocument.windowForSheet!.endSheet(infoWindow, returnCode: NSCancelButton)
		infoWindow.close()
	}
	
	func rawSoundData(inout settings: MADDriverSettings, handler: (NSData) -> MADErr, callback: (NSError?) -> Void) {
		var err: NSError? = nil
		if let theRec = PPDriver(library: globalMadLib, settings: &settings, error: &err) {
			theRec.cleanDriver()
			theRec.currentMusic = currentDocument.theMusic
			theRec.play()

			while let newData = theRec.directSave() {
				let anErr = handler(newData)
				if anErr != .NoErr {
					callback(createErrorFromMADErrorType(anErr))
					return
				}
			}
			
			callback(nil)
		} else {
			callback(err)
		}
	}
	
	func rawSoundData(inout theSet1: MADDriverSettings) -> NSMutableData? {
		let mutData = NSMutableData()
		var err: NSError? = nil
		
		func compileRawData(theData: NSData) -> MADErr {
			mutData.appendData(theData)
			return .NoErr
		}
		
		rawSoundData(&theSet1, handler: compileRawData) { (anErr) -> Void in
			if anErr != nil {
				err = anErr
				dispatch_async(dispatch_get_main_queue()) {
					NSAlert(error: err!).beginSheetModalForWindow(self.currentDocument.windowForSheet!, completionHandler: { (returnCode) -> Void in
						//Do nothing
					})
				}
			}
		}
		
		return err == nil ? mutData : nil
	}
	
	func rawBESoundData(inout theSet: MADDriverSettings) -> NSData? {
		if isLittleEndian {
			if let rsd = rawSoundData(&theSet) {
				if (theSet.outPutBits == 16) {
					let sndSize = rsd.length;
					let bePtr = UnsafeMutablePointer<UInt16>(rsd.mutableBytes)
					dispatch_apply(sndSize / 2, dispatch_get_global_queue(0, 0), { (i) -> Void in
						let iInt = Int(i)
						bePtr[iInt] = bePtr[iInt].bigEndian
						return
					})
				}
				return (rsd.copy() as! NSData)
			} else {
				return nil
			}
		} else {
			return (rawSoundData(&theSet)?.copy() as? NSData)
		}
	}
	
	func rawLESoundData(inout theSet: MADDriverSettings) -> NSData? {
		if !isLittleEndian {
			if let rsd = rawSoundData(&theSet) {
				if (theSet.outPutBits == 16) {
					let sndSize = rsd.length;
					let bePtr = UnsafeMutablePointer<UInt16>(rsd.mutableBytes)
					dispatch_apply(sndSize / 2, dispatch_get_global_queue(0, 0), { (i) -> Void in
						let iInt = Int(i)
						bePtr[iInt] = bePtr[iInt].littleEndian
						return
					})
				}
				return (rsd.copy() as! NSData)
			} else {
				return nil
			}
		} else {
			return rawSoundData(&theSet)?.copy() as? NSData
		}
	}
	
	private func applyMetadataToFileID(theID: AudioFileID) {
		//TODO: implement, but how?
	}
	
	private func saveMusic(waveToURL theURL: NSURL, inout theSett: MADDriverSettings) -> MADErr {
		if let saveData = rawLESoundData(&theSett) {
			var audioFile: AudioFileID = nil;
			var tmpChannels: UInt32 = 0
			
			switch (theSett.outPutMode) {
			case .MonoOutPut:
				tmpChannels = 1
				
			case .PolyPhonic:
				tmpChannels = 4
				
			default:
				tmpChannels = 2
			}
			
			var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: .SignedInteger | .Packed, bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
			
			var res = AudioFileCreate(URL: theURL, fileType: .WAVE, format: &asbd, flags: .EraseFile, audioFile: &audioFile);
			if (res != noErr) {
				if (audioFile != nil) {
					AudioFileClose(audioFile);
				}
				return MADErr.WritingErr;
			}
			
			var numBytes = UInt32(saveData.length)
			res = AudioFileWriteBytes(audioFile, 0, 0, &numBytes, saveData.bytes);
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
		} else {
			return .NeedMemory
		}
	}
	
	private func saveMusic(AIFFToURL theURL: NSURL, inout theSett: MADDriverSettings) -> MADErr {
		if let saveData = rawBESoundData(&theSett) {
			var audioFile: AudioFileID = nil;
			var tmpChannels: UInt32 = 0
			
			switch (theSett.outPutMode) {
			case .MonoOutPut:
				tmpChannels = 1
				
			case .PolyPhonic:
				tmpChannels = 4
				
			default:
				tmpChannels = 2
			}
			
			var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: .SignedInteger | .Packed | .BigEndian, bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
			
			var res = AudioFileCreate(URL: theURL, fileType: .AIFF, format: &asbd, flags: .EraseFile, audioFile: &audioFile)
			if (res != noErr) {
				if (audioFile != nil) {
					AudioFileClose(audioFile)
				}
				return MADErr.WritingErr
			}
			
			var numBytes = UInt32(saveData.length)
			res = AudioFileWriteBytes(audioFile, 0, 0, &numBytes, saveData.bytes)
			if (res != noErr) {
				if (audioFile != nil) {
					AudioFileClose(audioFile)
				}
				return MADErr.WritingErr
			}
			
			applyMetadataToFileID(audioFile)
			res = AudioFileClose(audioFile)
			if (res != noErr) {
				if (audioFile != nil) {
					AudioFileClose(audioFile)
				}
				return MADErr.WritingErr
			}
			
			return MADErr.NoErr
		} else {
			return MADErr.NeedMemory
		}
	}
	
	private func showExportSettingsWithExportType(expType: Int, URL theURL: NSURL) {
		exportSettings.resetToBestDriver()
		exportSettings.driverMode = .NoHardwareDriver;
		exportSettings.repeatMusic = false;
		exportController.settingsFromDriverSettings(exportSettings)
		
		self.currentDocument.windowForSheet!.beginSheet(exportWindow, completionHandler: { (returnCode) -> Void in
			if (returnCode == NSAlertDefaultReturn) {
				switch (expType) {
				case -1:
					let expObj = ExportObject(destination: theURL, exportBlock: { (theURL, errStr) -> MADErr in
						if errStr != nil {
							errStr.memory = nil
						}
						var expSett = self.exportSettings
						var theErr = self.saveMusic(AIFFToURL: theURL, theSett: &expSett)
						self.currentDocument.theDriver.endExport()
						return theErr
					})
					(NSApplication.sharedApplication().delegate as! AppDelegate).addExportObject(expObj)
					
				case -2:
					let expObj = ExportObject(destination: theURL, exportBlock: { (theURL, errStr) -> MADErr in
						var theErr = MADErr.NoErr;
						func generateAVMetadataInfo(oldMusicName: String, oldMusicInfo: String) -> [AVMetadataItem] {
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
							
							var musicNameQTUser = AVMutableMetadataItem();
							musicInfoQTUser.keySpace = AVMetadataKeySpaceQuickTimeUserData
							musicInfoQTUser.setKey(AVMetadataQuickTimeUserDataKeyFullName)
							musicInfoQTUser.setValue(oldMusicName)
							
							var musicInfoiTunes = AVMutableMetadataItem();
							musicInfoiTunes.keySpace = AVMetadataKeySpaceiTunes
							musicInfoiTunes.setKey(AVMetadataiTunesMetadataKeyUserComment)
							musicInfoiTunes.setValue(oldMusicInfo)
							
							var musicInfoQTMeta = AVMutableMetadataItem();
							musicInfoQTMeta.keySpace = AVMetadataKeySpaceQuickTimeMetadata
							musicInfoQTMeta.setKey(AVMetadataQuickTimeMetadataKeyInformation)
							musicInfoQTMeta.setValue(oldMusicInfo)
							
							return [titleName, dataInfo, musicInfoQTUser, musicInfoiTunes, musicInfoQTMeta, musicNameQTUser];
						}
						
						if errStr != nil {
							errStr.memory = nil
						}
						
						let oldMusicName = self.currentDocument.musicName;
						let oldMusicInfo = self.currentDocument.musicInfo;
						let oldURL = self.currentDocument.fileURL;
						let tmpURL = NSFileManager.defaultManager().URLForDirectory(.ItemReplacementDirectory, inDomain:.UserDomainMask, appropriateForURL:oldURL, create:true, error:nil)!.URLByAppendingPathComponent( (oldMusicName != "" ? oldMusicName : "untitled") + ".aiff", isDirectory: false)
						var expSett = self.exportSettings
						theErr = self.saveMusic(AIFFToURL: tmpURL, theSett: &expSett)
						self.currentDocument.theDriver.endExport()
						if (theErr != MADErr.NoErr) {
							if (errStr != nil) {
								errStr.memory = "Unable to save temporary file to \(tmpURL.path), error \(theErr)."
							}
							
							return theErr;
						}
						
						var exportMov = AVAsset.assetWithURL(tmpURL) as? AVAsset
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
					(NSApplication.sharedApplication().delegate as! AppDelegate).addExportObject(expObj)
					
				default:
					self.currentDocument.theDriver.exporting = false
				}
			}
		})
	}
	
	@IBAction func exportMusicAs(sender: AnyObject!) {
		let tag = (sender as! NSMenuItem).tag
		self.currentDocument.theDriver.beginExport()
		let savePanel = NSSavePanel()
		let musicName = self.currentDocument.musicName;
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
			
			savePanel.beginSheetModalForWindow(self.currentDocument.windowForSheet!, completionHandler: { (result) -> Void in
				if result == NSFileHandlingPanelOKButton {
					self.showExportSettingsWithExportType(-1, URL: savePanel.URL!)
				} else {
					self.currentDocument.theDriver.endExport()
				}
			})
			
			break;
			
		case -2:
			//MP4
			savePanel.allowedFileTypes = [AVFileTypeAppleM4A]
			savePanel.title = "Export as MPEG-4 Audio"
			savePanel.beginSheetModalForWindow(self.currentDocument.windowForSheet!, completionHandler: { (result) -> Void in
				if result == NSFileHandlingPanelOKButton {
					self.showExportSettingsWithExportType(-2, URL: savePanel.URL!)
				} else {
					self.currentDocument.theDriver.endExport()
				}
			})
			break;
			
		default:
			
			if (tag > Int(globalMadLib.pluginCount) || tag < 0) {
				NSBeep();
				self.currentDocument.theDriver.endExport()
				
				return;
			}
			let tmpObj = globalMadLib[tag];
			
			savePanel.allowedFileTypes = tmpObj.UTITypes
			savePanel.title = "Export as \(tmpObj.menuName)"
			
			savePanel.beginSheetModalForWindow(self.currentDocument.windowForSheet!, completionHandler: { (result) -> Void in
				if result == NSFileHandlingPanelOKButton {
					var expObj = ExportObject(destination: savePanel.URL!, exportBlock: { (theURL, errStr) -> MADErr in
						var theErr = self.currentDocument.theMusic.exportMusicToURL(theURL, format: tmpObj.type, library: globalMadLib)
						self.currentDocument.theDriver.endExport()
						return theErr
					})
				} else {
					self.currentDocument.theDriver.exporting = false
				}
			})
			break;
		}
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
