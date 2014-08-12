//
//  AppDelegate.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/11/14.
//
//

import Cocoa
import PlayerPROKit
import PlayerPROCore
import AVFoundation

class AppDelegate: NSObject, NSApplicationDelegate, PPSoundSettingsViewControllerDelegate, NSTableViewDelegate, NSToolbarDelegate, NSTableViewDataSource {
	@IBOutlet var window: NSWindow! = nil
	@IBOutlet var loopButton: NSButton!
	@IBOutlet var playButton: NSButton!
	@IBOutlet var stopButton: NSButton!
	@IBOutlet var songCurTime: NSTextField!
	@IBOutlet var songTotalTime: NSTextField!
	@IBOutlet var songPos: NSSlider!
	@IBOutlet var pauseButton: NSButton!
	
	@IBOutlet var aboutPlugInMenu: NSMenu!
	
	@IBOutlet var musicExportMenu: NSMenu!
	
	@IBOutlet var infoDrawer: NSDrawer!
	@IBOutlet var fileName: NSTextField!
	@IBOutlet var internalName: NSTextField!
	@IBOutlet var fileSize: NSTextField!
	@IBOutlet var musicInstrument: NSTextField!
	@IBOutlet var musicPatterns: NSTextField!
	@IBOutlet var musicPlugType: NSTextField!
	@IBOutlet var musicSignature: NSTextField!
	@IBOutlet var fileLocation: NSTextField!
	
	@IBOutlet var tableView: NSTableView!
	@IBOutlet var musicListController: NSArrayController!
	@IBOutlet var exportWindow: NSWindow!
	@IBOutlet var pauseDockMenuItem: NSMenuItem!
	var music: PPMusicObject!
	
	@IBOutlet var toolsPanel: NSPanel!
	var timeChecker: NSTimer!
	var madDriver: PPDriver!
	var madLib: PPLibrary!
	var paused = true
	var isQuitting = false
	var instrumentController: PPInstrumentWindowController! = nil
	
	var exportSettings = MADDriverSettings()
	@IBOutlet var exportSettingsBox: NSBox!
	var exportController: PPSoundSettingsViewController!

	var musicList = PPMusicList()
	var preferences: PPPreferences! = nil
	var plugInInfos = [PPPlugInInfo]()
	
	var currentlyPlayingIndex	= PPCurrentlyPlayingIndex()
	var previouslyPlayingIndex	= PPCurrentlyPlayingIndex()

	@IBAction func showMusicList(sender: AnyObject!) {
		self.window.makeKeyAndOrderFront(sender)
	}
	
	func clearMusic() {
		
	}
	
	func setTitleForSongLabelBasedOnMusic() {
		
	}
	
	func loadMusicURL(musicToLoad: NSURL!, error theErr: NSErrorPointer? = nil) -> Bool {
		
		var fileType: [Int8] = [0,0,0,0,0]
		var theOSErr = MADErr.NoErr;
		if (self.music != nil) {
			madDriver.stop()
			madDriver.stopDriver()
		}
		
		theOSErr = madLib.identifyFileAtURL(musicToLoad, type: &fileType)
		
		if (theOSErr != .NoErr) {
			theErr?.memory = CreateErrorFromMADErrorType(theOSErr);
			self.paused = true;
			self.clearMusic();
			return false;
		}
		
		self.music = PPMusicObject(URL:musicToLoad, library:madLib)
		if (self.music == nil) {
			theOSErr = .ReadingErr;
		}
		
		if (theOSErr != .NoErr) {
			theErr?.memory = CreateErrorFromMADErrorType(theOSErr);
			self.paused = true;
			self.clearMusic();
			return false;
		}
		
		music.attachToDriver(madDriver)
		madDriver.play()
		self.paused = false;
		
		var fT = 0
		var cT = 0
		madDriver.getMusicStatusWithCurrentTime(&cT, totalTime: &fT)
		songPos.maxValue = Double(fT)
		songPos.minValue = 0.0
		setTitleForSongLabelBasedOnMusic()
		songTotalTime.integerValue = fT
		
		NSNotificationCenter.defaultCenter().postNotificationName(PPMusicDidChange, object: self)
		
		theErr?.memory = nil;
		return true;
	}

	@IBAction func showPreferences(sender: AnyObject!) {
		if (preferences == nil) {
			preferences = PPPreferences()
			preferences.window.center()
		}
		preferences.showWindow(sender)
	}

	override func observeValueForKeyPath(keyPath: String!, ofObject object: AnyObject!, change: [NSObject : AnyObject]!, context: UnsafeMutablePointer<()>) {
		if (keyPath == "paused") {
			var boolVal = (change[NSKeyValueChangeNewKey] as NSNumber);
			
			//[pauseButton highlight:[boolVal boolValue]];
			switch (boolVal.boolValue) {
				
			case true:
				pauseButton.state = NSOnState
				pauseDockMenuItem.state = NSOffState
				break;
				
			case false:
				fallthrough
			default:
				pauseButton.state = NSOffState
				pauseDockMenuItem.state = NSOnState
				break;
			}
		}
		//[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
	}
	
	func loadMusicFromCurrentlyPlayingIndexWithError(theErr:NSErrorPointer?) -> Bool {
		currentlyPlayingIndex.playbackURL = musicList.URLAtIndex(currentlyPlayingIndex.index)
		var isGood = loadMusicURL(currentlyPlayingIndex.playbackURL, error: theErr)
		currentlyPlayingIndex.movePlayingIndexToOtherIndex(previouslyPlayingIndex)
		return isGood;
	}
	
	func doubleClickMusicList() {
		var err:NSError? = nil
		self.currentlyPlayingIndex.index = tableView.selectedRow;
		if (loadMusicFromCurrentlyPlayingIndexWithError(&err) == false) {
			NSAlert(error:err).runModal()
		}
	}

	func selectCurrentlyPlayingMusic() {
		if (self.currentlyPlayingIndex.index >= 0) {
			//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
			selectMusicAtIndex(currentlyPlayingIndex.index)
		}
	}
	
	func selectMusicAtIndex(anIdx: Int) {
		if (anIdx < 0 || anIdx >= musicList.countOfMusicList) {
			NSBeep();
			return;
		}
		let idx = NSIndexSet(index: anIdx);
		tableView.selectRowIndexes(idx, byExtendingSelection: false)
		tableView.scrollRowToVisible(anIdx)
	}
	
	func addMusicToMusicList(theURL: NSURL, loadIfPreferencesAllow load:Bool = true) {
		self.willChangeValueForKey(kMusicListKVO)
		var okayMusic = musicList.addMusicURL(theURL)
		self.didChangeValueForKey(kMusicListKVO)
		if (!okayMusic) {
			var similarMusicIndex = musicList.indexOfObjectSimilarToURL(theURL)
			if (similarMusicIndex == NSNotFound) {
				return;
			}
			if (load && NSUserDefaults.standardUserDefaults().boolForKey(PPLoadMusicAtMusicLoad)) {
				self.currentlyPlayingIndex.index = similarMusicIndex;
				selectCurrentlyPlayingMusic()
				var err: NSError? = nil
				if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
					NSAlert(error: err).runModal()
				}
			} else {
				selectMusicAtIndex(similarMusicIndex)
			}
		} else if (load && NSUserDefaults.standardUserDefaults().boolForKey(PPLoadMusicAtMusicLoad)) {
			self.currentlyPlayingIndex.index = musicList.countOfMusicList - 1;
			//currentlyPlayingIndex.playbackURL = [musicList URLAtIndex:currentlyPlayingIndex.index];
			selectCurrentlyPlayingMusic()
			var err: NSError? = nil
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error: err).runModal()
			}
		} else {
			selectMusicAtIndex(musicList.countOfMusicList - 1)
		}
	}
	
	
	func songIsDonePlaying() {
		var err: NSError? = nil
		var userDefaults = NSUserDefaults.standardUserDefaults()
		switch (PPPlaylistMode.fromRaw(userDefaults.integerForKey(PPAfterPlayingMusic))!) {
			
		case PPPlaylistMode.LoopMusic:
			madDriver.musicPosition = 0;
			madDriver.play()
			break;
			
		case PPPlaylistMode.LoadNext:
			if (musicList.countOfMusicList > ++self.currentlyPlayingIndex.index) {
				selectCurrentlyPlayingMusic()
				if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
					NSAlert(error: err).runModal()
				}
			} else {
				if (userDefaults.boolForKey(PPLoopMusicWhenDone)) {
					currentlyPlayingIndex.index = 0;
					selectCurrentlyPlayingMusic()
					if (!self.loadMusicFromCurrentlyPlayingIndexWithError(&err))
					{
						NSAlert(error: err).runModal()
					}
				} else {
					madDriver.stop()
					madDriver.cleanDriver()
					if (userDefaults.boolForKey(PPGotoStartupAfterPlaying)) {
						madDriver.musicPosition = 0;
					}
				}
			}
			break;
			
		case .LoadRandom:
			self.currentlyPlayingIndex.index = random() % musicList.countOfMusicList;
			selectCurrentlyPlayingMusic()
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error: err).runModal()
			}
			
			break;
			
		case .StopPlaying:
			fallthrough
		default:
			madDriver.stop()
			madDriver.cleanDriver()
			if (userDefaults.boolForKey(PPGotoStartupAfterPlaying)) {
				madDriver.musicPosition = 0;
			}
			self.paused = true
			break;
		}
	}
	
	func updateMusicStats(theTimer: NSTimer!) {
		if (self.music) {
			var fT = 0
			var cT = 0;
			madDriver.getMusicStatusWithCurrentTime(&cT, totalTime: &fT)
			if (madDriver.isDonePlayingMusic() && paused == false && madDriver.isExporting == false) {
				songIsDonePlaying()
				madDriver.getMusicStatusWithCurrentTime(&cT, totalTime: &fT)
			}
			songPos.doubleValue = Double(cT)
			songCurTime.integerValue = cT
		}
	}
	
	func saveMusicToURL(tosave: NSURL) {
		music.saveMusicToURL(tosave)
	}

	class func generateAVMetadataInfo(oldMusicName: String, oldMusicInfo: String) -> [AVMutableMetadataItem] {
		var titleName = AVMutableMetadataItem()
		titleName.keySpace = AVMetadataKeySpaceCommon
		titleName.setKey(AVMetadataCommonKeyTitle)
		titleName.setValue(oldMusicName)
		
		var dataInfo = AVMutableMetadataItem()
		dataInfo.keySpace = AVMetadataKeySpaceCommon;
		dataInfo.setKey(AVMetadataCommonKeySoftware)
		dataInfo.setValue("PlayerPRO Player")
		
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
	
	func rawSoundData(theSet: UnsafeMutablePointer<MADDriverSettings>) -> NSMutableData? {
		var err = MADErr.NoErr;
		var theRec = PPDriver(library: madLib, settings: theSet, error: &err)
		
		if (theRec == nil) {
			dispatch_async(dispatch_get_main_queue(), ({
				var NSerr = CreateErrorFromMADErrorType(.UnknownErr);
				NSAlert(error:NSerr).runModal()
			}))
			
			return nil;
		}
		
		theRec.cleanDriver()
		theRec.currentMusic = music
		
		var soundPtr:UnsafeMutablePointer<()> = nil;
		var full = theRec.audioDataLength();
		if (theSet.memory.outPutBits == 16) {
			full *= 2;
		} else if (theSet.memory.outPutBits == 20 || theSet.memory.outPutBits == 24 ) {
			full *= 3;
		}
		
		switch (theSet.memory.outPutMode) {
		case .DeluxeStereoOutPut:
			fallthrough
		case .StereoOutPut:
			full *= 2;
			
		case .PolyPhonic:
			full *= 4;
			
		default:
			break;
		}
		
		theRec.play()
		
		var mutData = NSMutableData();
		soundPtr = calloc(UInt(full), 1);
		
		while (theRec.directSaveToPointer(soundPtr, settings:theSet)) {
			mutData.appendBytes(soundPtr, length: full)
		}
		
		free(soundPtr);
		return mutData;
	}
	
	@IBAction func okayExportSettings(sender: AnyObject!) {
		window.endSheet(exportWindow, returnCode: NSAlertDefaultReturn)
		exportWindow.close()
	}
	
	@IBAction func cancelExportSettings(sender: AnyObject!) {
		window.endSheet(exportWindow, returnCode: NSAlertAlternateReturn)
		exportWindow.close()
	}

	func tableView(tableView: NSTableView!, toolTipForCell cell: NSCell!, rect: NSRectPointer, tableColumn: NSTableColumn!, row: Int, mouseLocation: NSPoint) -> String! {
		if (row >= 0 || row <= musicList.countOfMusicList) {
			return nil;
		}
		return musicList.URLAtIndex(row).path
	}
	
	// PPSoundSettingsViewController delegate methods
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
