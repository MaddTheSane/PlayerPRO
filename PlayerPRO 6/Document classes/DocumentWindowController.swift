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
import SwiftAudioAdditions

class DocumentWindowController: NSWindowController {
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
	fileprivate var exportSettings = MADDriverSettings.new()

	let exportController = SoundSettingsViewController.newSoundSettingWindow()!

	override func awakeFromNib() {
		super.awakeFromNib()
		exportController.delegate = self;
		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		exportSettingsBox.contentView = exportController.view

	}
	
	override var windowNibName: NSNib.Name? {
		// Override returning the nib file name of the document
		// If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
		return NSNib.Name(rawValue: "PPDocument")
	}
	
	@IBAction func showBoxEditor(_ sender: AnyObject!) {
		editorsTab.selectTabViewItem(withIdentifier: "Box")
	}
	
	@IBAction func showClassicEditor(_ sender: AnyObject!) {
		editorsTab.selectTabViewItem(withIdentifier: "Classic")
	}
	
	@IBAction func showDigitalEditor(_ sender: AnyObject!) {
		editorsTab.selectTabViewItem(withIdentifier: "Digital")
	}
	
	@IBAction func showWavePreview(_ sender: AnyObject!) {
		editorsTab.selectTabViewItem(withIdentifier: "Wave")
	}
	
	@IBAction func okayExportSettings(_ sender: AnyObject!) {
		currentDocument.windowForSheet!.endSheet(exportWindow, returnCode: NSApplication.ModalResponse.alertFirstButtonReturn)
		exportWindow.close()
	}
	
	@IBAction func cancelExportSettings(_ sender: AnyObject!) {
		currentDocument.windowForSheet!.endSheet(exportWindow, returnCode: NSApplication.ModalResponse.alertSecondButtonReturn)
		exportWindow.close()
	}
	
	@IBAction func showMusicInfo(_ sender: AnyObject?) {
		infoInfoField.stringValue = currentDocument.musicInfo
		infoNameField.stringValue = currentDocument.musicName
		
		currentDocument.windowForSheet!.beginSheet(infoWindow, completionHandler: { (response) -> Void in
			switch response {
			case NSApplication.ModalResponse.alertFirstButtonReturn:
				let um = self.currentDocument.undoManager!
				um.beginUndoGrouping()
				
				//let aDoc: (DocumentWindowController) = um.prepareWithInvocationTarget(self) as (DocumentWindowController)
				
				if self.currentDocument.musicInfo != self.infoInfoField.stringValue {
					um.registerUndo(withTarget: self.currentDocument, selector: #selector(setter: PPDocument.musicInfo), object: self.currentDocument.musicInfo)
					//aDoc.musicInfo = self.currentDocument.musicInfo
					self.currentDocument.musicInfo = self.infoInfoField.stringValue
				}
				
				if self.currentDocument.musicName != self.infoNameField.stringValue {
					um.registerUndo(withTarget: self.currentDocument, selector: #selector(setter: PPDocument.musicName), object: self.currentDocument.musicName)

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
	
	@IBAction func okayMusicInfo(_ sender: AnyObject?) {
		currentDocument.windowForSheet!.endSheet(infoWindow, returnCode: NSApplication.ModalResponse.alertFirstButtonReturn)
		infoWindow.close()
	}
	
	@IBAction func cancelMusicInfo(_ sender: AnyObject?) {
		currentDocument.windowForSheet!.endSheet(infoWindow, returnCode: NSApplication.ModalResponse.cancel)
		infoWindow.close()
	}
	
	func rawSoundData(_ settings: inout MADDriverSettings, handler: (Data) throws -> Void) throws {
		var settings = settings
		let theRec =  try PPDriver(library: globalMadLib, settings: &settings)
		theRec.cleanDriver()
		theRec.currentMusic = currentDocument.theMusic
		theRec.play()
		
		while let newData = theRec.directSave() {
			try handler(newData)
		}
	}
	
	private func applyMetadata(to theID: ExtAudioFile) {
		//TODO: implement, but how?
	}
	
	private func saveMusic(waveToURL theURL: URL, theSett: inout MADDriverSettings) -> MADErr {
		do {
			var audioFile: ExtAudioFile!
			let tmpChannels: UInt32
			
			switch theSett.outPutMode {
			case .MonoOutPut:
				tmpChannels = 1
				
			case .PolyPhonic:
				tmpChannels = 4
				
			default:
				tmpChannels = 2
			}
			
			var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
			var realFormat = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed, .nativeEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
			
			audioFile = try ExtAudioFile(create: theURL, fileType: .WAVE, streamDescription: &asbd, flags: .eraseFile)
			
			audioFile.clientDataFormat = realFormat
			
			func handler(data data1: Data) throws {
				try data1.withUnsafeBytes { (toWriteBytes: UnsafePointer<UInt8>) -> Void in
					let toWriteSize = data1.count
					
					var audBufList = AudioBufferList()
					audBufList.mNumberBuffers = 1
					audBufList.mBuffers.mNumberChannels = tmpChannels
					audBufList.mBuffers.mDataByteSize = UInt32(toWriteSize)
					audBufList.mBuffers.mData = UnsafeMutableRawPointer(mutating: toWriteBytes)
					
					try audioFile.write(frames: UInt32(toWriteSize) / realFormat.mBytesPerFrame, data: &audBufList)
				}
			}
			
			try rawSoundData(&theSett, handler: handler)
			applyMetadata(to: audioFile)
		} catch let error as MADErr {
			return error
		} catch {
			return MADErr.writingErr
		}
		
		return MADErr.noErr
	}
	
	private func saveMusic(AIFFToURL theURL: URL, theSett: inout MADDriverSettings) -> MADErr {
		do {
			let tmpChannels: UInt32
			
			switch theSett.outPutMode {
			case .MonoOutPut:
				tmpChannels = 1
				
			case .PolyPhonic:
				tmpChannels = 4
				
			default:
				tmpChannels = 2
			}
			
			var asbd = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed, .bigEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
			var realFormat = AudioStreamBasicDescription(sampleRate: Float64(theSett.outPutRate), formatFlags: [.signedInteger, .packed, .nativeEndian], bitsPerChannel: UInt32(theSett.outPutBits), channelsPerFrame: tmpChannels)
			
			let audioFile = try ExtAudioFile(create: theURL, fileType: .AIFF, streamDescription: &asbd, flags: .eraseFile)
			
			audioFile.clientDataFormat = realFormat
			
			func handler(data data1: Data) throws {
				try data1.withUnsafeBytes { (toWriteBytes: UnsafePointer<UInt8>) -> Void in
					let toWriteSize = data1.count
					
					var audBufList = AudioBufferList()
					audBufList.mNumberBuffers = 1
					audBufList.mBuffers.mNumberChannels = tmpChannels
					audBufList.mBuffers.mDataByteSize = UInt32(toWriteSize)
					audBufList.mBuffers.mData = UnsafeMutableRawPointer(mutating: toWriteBytes)
					
					try audioFile.write(frames: UInt32(toWriteSize) / realFormat.mBytesPerFrame, data: &audBufList)
				}
			}
			
			try rawSoundData(&theSett, handler: handler)
			applyMetadata(to: audioFile)
		} catch let error as MADErr {
			return error
		} catch {
			return MADErr.writingErr
		}
		
		return MADErr.noErr
	}
	
	private func showExportSettingsWithExportType(_ expType: Int, URL theURL: URL) {
		exportSettings.resetToBestDriver()
		exportSettings.driverMode = .NoHardwareDriver;
		exportSettings.repeatMusic = false;
		exportController.settingsFromDriverSettings(exportSettings)
		
		self.currentDocument.windowForSheet!.beginSheet(exportWindow, completionHandler: { (returnCode) -> Void in
			if returnCode == NSApplication.ModalResponse.alertFirstButtonReturn {
				switch (expType) {
				case -1:
					let expObj = ExportObject(destination: theURL, exportBlock: { (theURL, errStr) -> MADErr in
						errStr?.pointee = nil
						var expSett = self.exportSettings
						var theErr = self.saveMusic(AIFFToURL: theURL, theSett: &expSett)
						self.currentDocument.theDriver.endExport()
						return theErr
					})
					(NSApplication.shared.delegate as! AppDelegate).add(exportObject: expObj)
					
				case -2:
					let expObj = ExportObject(destination: theURL, exportBlock: { (theURL, errStr) -> MADErr in
						//do {
						var theErr = MADErr.noErr;
						func generateAVMetadataInfo(_ oldMusicName: String, oldMusicInfo: String) -> [AVMetadataItem] {
							let titleName = AVMutableMetadataItem()
							titleName.keySpace = AVMetadataKeySpace.common
							titleName.key = AVMetadataKey.commonKeyTitle as NSString
							titleName.value = oldMusicName as NSString
							
							let dataInfo = AVMutableMetadataItem()
							dataInfo.keySpace = AVMetadataKeySpace.quickTimeUserData
							dataInfo.key = AVMetadataKey.quickTimeUserDataKeySoftware as NSString
							dataInfo.value = "PlayerPRO 6" as NSString
							dataInfo.locale = Locale(identifier: "en")
							
							let musicInfoQTUser = AVMutableMetadataItem();
							musicInfoQTUser.keySpace = AVMetadataKeySpace.quickTimeUserData
							musicInfoQTUser.key = AVMetadataKey.quickTimeUserDataKeyInformation as NSString
							musicInfoQTUser.value = (oldMusicInfo) as NSString
							musicInfoQTUser.locale = Locale.current

							let musicNameQTUser = AVMutableMetadataItem()
							musicNameQTUser.keySpace = AVMetadataKeySpace.quickTimeUserData
							musicNameQTUser.key = AVMetadataKey.quickTimeUserDataKeyFullName as NSString
							musicNameQTUser.value = oldMusicName as NSString
							musicNameQTUser.locale = Locale.current
							
							let musicInfoiTunes = AVMutableMetadataItem()
							musicInfoiTunes.keySpace = AVMetadataKeySpace.iTunes
							musicInfoiTunes.key = AVMetadataKey.iTunesMetadataKeyUserComment as NSString
							musicInfoiTunes.value = oldMusicInfo as NSString
							
							let musicInfoQTMeta = AVMutableMetadataItem();
							musicInfoQTMeta.keySpace = AVMetadataKeySpace.quickTimeMetadata
							musicInfoQTMeta.key = AVMetadataKey.quickTimeMetadataKeyInformation as NSString
							musicInfoQTMeta.value = oldMusicInfo as NSString
							musicInfoQTMeta.locale = Locale.current
							
							return [titleName, dataInfo, musicInfoQTUser, musicInfoiTunes, musicInfoQTMeta, musicNameQTUser];
						}
						
						errStr?.pointee = nil
						
						let oldMusicName = self.currentDocument.musicName;
						let oldMusicInfo = self.currentDocument.musicInfo;
						let oldURL = self.currentDocument.fileURL;
						let tmpURL = (try! FileManager.default.url(for: .itemReplacementDirectory, in:.userDomainMask, appropriateFor:oldURL, create:true)).appendingPathComponent( (oldMusicName != "" ? oldMusicName : "untitled") + ".aiff", isDirectory: false)
						var expSett = self.exportSettings
						theErr = self.saveMusic(AIFFToURL: tmpURL, theSett: &expSett)
						self.currentDocument.theDriver.endExport()
						defer {
							do {
								try FileManager.default.removeItem(at: tmpURL)
							} catch _ {
								
							}
						}
						if (theErr != MADErr.noErr) {
							errStr?.pointee = "Unable to save temporary file to \(tmpURL.path), error \(theErr)."  as NSString
							
							return theErr;
						}
						
						let exportMov = AVAsset(url: tmpURL)
						let metadataInfo = generateAVMetadataInfo(oldMusicName, oldMusicInfo: oldMusicInfo)
						
						let tmpsession = AVAssetExportSession(asset: exportMov, presetName: AVAssetExportPresetAppleM4A)
						if (tmpsession == nil) {
							errStr?.pointee = "Export session creation for \(oldMusicName) failed." as NSString
							return .writingErr;
						}
						let session = tmpsession!
						do {
							try FileManager.default.removeItem(at: theURL)
						} catch {
							
						}
						session.outputURL = theURL;
						session.outputFileType = AVFileType.m4a;
						session.metadata = metadataInfo;
						let sessionWaitSemaphore = DispatchSemaphore(value: 0);
						session.exportAsynchronously(completionHandler: { () -> Void in
							sessionWaitSemaphore.signal();
							return;
						})
						_ = sessionWaitSemaphore.wait(timeout: DispatchTime.distantFuture)
						
						let didFinish = session.status == .completed;
						
						if (didFinish) {
							return .noErr;
						} else {
							errStr?.pointee = "export of \"\(oldMusicName)\" failed." as NSString
							return .writingErr;
						}
						//} catch _ {}
					})
					(NSApplication.shared.delegate as! AppDelegate).add(exportObject: expObj)
					
				default:
					self.currentDocument.theDriver.isExporting = false
				}
			}
		})
	}
	
	@IBAction func exportMusicAs(_ sender: AnyObject!) {
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
			savePanel.allowedFileTypes = [AVFileType.aiff.rawValue]
			savePanel.title = "Export as AIFF audio"
			
			savePanel.beginSheetModal(for: self.currentDocument.windowForSheet!, completionHandler: { (result) -> Void in
				if result.rawValue == NSFileHandlingPanelOKButton {
					self.showExportSettingsWithExportType(-1, URL: savePanel.url!)
				} else {
					self.currentDocument.theDriver.endExport()
				}
			})
			
			break;
			
		case -2:
			//MP4
			savePanel.allowedFileTypes = [AVFileType.m4a.rawValue]
			savePanel.title = "Export as MPEG-4 Audio"
			savePanel.beginSheetModal(for: self.currentDocument.windowForSheet!, completionHandler: { (result) -> Void in
				if result.rawValue == NSFileHandlingPanelOKButton {
					self.showExportSettingsWithExportType(-2, URL: savePanel.url!)
				} else {
					self.currentDocument.theDriver.endExport()
				}
			})
			break;
			
		default:
			
			if (tag > Int(globalMadLib.pluginCount) || tag < 0) {
				NSSound.beep();
				self.currentDocument.theDriver.endExport()
				
				return;
			}
			let tmpObj = globalMadLib[tag];
			
			savePanel.allowedFileTypes = tmpObj.UTITypes
			savePanel.title = "Export as \(tmpObj.menuName)"
			
			savePanel.beginSheetModal(for: self.currentDocument.windowForSheet!, completionHandler: { (result) -> Void in
				if result.rawValue == NSFileHandlingPanelOKButton {
					let expObj = ExportObject(destination: savePanel.url!, exportBlock: { (theURL, errStr) -> MADErr in
						var theErr = MADErr.noErr
						do {
						try self.currentDocument.theMusic.exportMusic(to: theURL, format: tmpObj.type, library: globalMadLib)
						} catch {
							if let error = error as? MADErr {
								theErr = error
							} else {
								theErr = .unknownErr
							}
						}
						self.currentDocument.theDriver.endExport()
						return theErr
					})
					(NSApplication.shared.delegate as! AppDelegate).add(exportObject: expObj)
				} else {
					self.currentDocument.theDriver.isExporting = false
				}
			})
			break;
		}
	}
}

extension DocumentWindowController: SoundSettingsViewControllerDelegate {
	func soundView(_ view: SoundSettingsViewController, bitsDidChange bits: Int16) {
		exportSettings.outPutBits = bits;
	}
	
	func soundView(_ view: SoundSettingsViewController, rateDidChange rat: UInt32) {
		exportSettings.outPutRate = rat;
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbDidChangeActive isAct: Bool) {
		exportSettings.Reverb = isAct;
	}
	
	func soundView(_ view: SoundSettingsViewController, oversamplingDidChangeActive isAct: Bool) {
		if (!isAct) {
			exportSettings.oversampling = 1;
		}
	}
	
	func soundView(_ view: SoundSettingsViewController, stereoDelayDidChangeActive isAct: Bool) {
		if (!isAct) {
			exportSettings.MicroDelaySize = 0;
		}
	}
	
	func soundView(_ view: SoundSettingsViewController, surroundDidChangeActive isAct: Bool) {
		exportSettings.surround = isAct;
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbStrengthDidChange rev: Int32) {
		exportSettings.ReverbStrength = rev;
	}
	
	func soundView(_ view: SoundSettingsViewController, reverbSizeDidChange rev: Int32) {
		exportSettings.ReverbSize = rev;
	}
	
	func soundView(_ view: SoundSettingsViewController, oversamplingAmountDidChange ovs: Int32) {
		exportSettings.oversampling = ovs;
	}
	
	func soundView(_ view: SoundSettingsViewController, stereoDelayAmountDidChange std: Int32) {
		exportSettings.MicroDelaySize = std;
	}
}
