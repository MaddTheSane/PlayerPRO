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

func generateAVMetadataInfo(oldMusicName: String, oldMusicInfo: String) -> [AVMutableMetadataItem] {
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
	private var isQuitting = false
	var instrumentController: PPInstrumentWindowController! = nil
	
	var exportSettings = MADDriverSettings()
	@IBOutlet var exportSettingsBox: NSBox!
	var exportController: PPSoundSettingsViewController!

	var musicList = PPMusicList()
	var preferences: PPPreferences! = nil
	var plugInInfos = [PPPlugInInfo]()
	
	private var currentlyPlayingIndex	= PPCurrentlyPlayingIndex()
	private var previouslyPlayingIndex	= PPCurrentlyPlayingIndex()

	private var musicName = ""
	private var musicInfo = ""
	private var selMusFromList = 0
	
	private var _trackerDict: [String: [String!]]! = nil
	private var _trackerUTIs: [String!] = []
	
	var trackerDict: [String: [String!]] { get {
		if _trackerDict == nil || _trackerDict.count + 2 != Int(madLib.pluginCount) {
			var localMADKName = NSLocalizedString("PPMADKFile", tableName: "InfoPlist", comment: "MADK Tracker")
			var localGenericMADName = NSLocalizedString("Generic MAD tracker", comment: "Generic MAD tracker")
			var tmpTrackerDict: [String: [String!]] = [localMADKName: [MADNativeUTI as String], localGenericMADName: [MADGenericUTI as String]]
			
			var i: UInt = 0
			for (i = 0; i < madLib.pluginCount; i++) {
				var obj = madLib.pluginAtIndex(i)
				tmpTrackerDict[obj.menuName as String] = (obj.UTItypes) as? [String!]
			}
			_trackerDict = tmpTrackerDict
		}
		return _trackerDict
		}}

	var trackerUTIs: [String!] { get {
		if _trackerUTIs.isEmpty {
			let arrayOfUTIs = trackerDict.values
			var toAddUTI: [String!] = []
			for anArray in arrayOfUTIs {
				toAddUTI += anArray
			}
			_trackerUTIs = toAddUTI
		}
		return _trackerUTIs
		}}
	
	@IBAction func showMusicList(sender: AnyObject!) {
		self.window.makeKeyAndOrderFront(sender)
	}
	
	func clearMusic() {
		if (music != nil) {
			madDriver.stop()
			madDriver.cleanDriver()
		}
		
		self.paused = true;
		currentlyPlayingIndex.index = -1;
		currentlyPlayingIndex.playbackURL = nil;
		currentlyPlayingIndex.movePlayingIndexToOtherIndex(previouslyPlayingIndex)
		self.music = PPMusicObject();
		setTitleForSongLabelBasedOnMusic()
		NSNotificationCenter.defaultCenter().postNotificationName(PPMusicDidChange, object:self)
		music.attachToDriver(madDriver)
	}
	
	private func setTitleForSongLabelBasedOnMusic() {
		self.musicName = music.internalFileName;
		self.musicInfo = music.madInformation
	}
	
	func loadMusicURL(musicToLoad: NSURL!, error theErr: NSErrorPointer? = nil) -> Bool {
		var fileType = [Int8](count: 5, repeatedValue: 0)
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
	
	private func changeValueForMusicListKey(theClosure: () -> Void) {
		self.willChangeValueForKey(kMusicListKVO)
		theClosure()
		self.didChangeValueForKey(kMusicListKVO)
	}
	
	@IBAction func sortMusicList(sender: AnyObject!) {
		
		changeValueForMusicListKey({
			self.musicList.sortMusicListByName()
		})
		musicListContentsDidMove()
	}

	
	func updateMusicStats(theTimer: NSTimer!) {
		if (self.music != nil) {
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

	private func musicListContentsDidMove() {
		var i = 0;
		if (self.currentlyPlayingIndex.index != -1) {
			for (i = 0; i < musicList.countOfMusicList; i++) {
				if (self.currentlyPlayingIndex.playbackURL == musicList.URLAtIndex(i)) {
					self.currentlyPlayingIndex.index = i;
					break;
				}
			}
		}
		if (self.previouslyPlayingIndex.index != -1) {
			for (i = 0; i < musicList.countOfMusicList; i++) {
				if (self.previouslyPlayingIndex.playbackURL == musicList.URLAtIndex(i)) {
					self.previouslyPlayingIndex.index = i;
					break;
				}
			}
		}
	}
	
	@IBAction func removeSelectedMusic(sender: AnyObject!) {
		var selMusic = tableView.selectedRowIndexes
		changeValueForMusicListKey({
			self.musicList.removeObjectsInMusicListAtIndexes(selMusic)
		})
		musicListContentsDidMove()
	}
	
	@IBAction func newMusic(sender: AnyObject) {
		clearMusic()
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
	
	func musicListDidChange() {
		
	}
	
	func handleFile(theURL1: NSURL!, ofType theUTI: String) -> Bool {
		var sharedWorkspace = NSWorkspace.sharedWorkspace()
		var theURL = theURL1
		if (theUTI  == MADGenericUTI) {
			var invExt = NSLocalizedString("Invalid Extension", comment: "Invalid Extension")
			var invExtDes = NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description")
			var renameFile =  NSLocalizedString("Rename", comment: "rename file")
			var openFile = NSLocalizedString("Open", comment: "Open a file")
			var cancelOp = NSLocalizedString("Cancel", comment: "Cancel")
			var retVal = PPRunInformationalAlertPanel(NSLocalizedString("Invalid Extension", comment: "Invalid extension"), message: NSLocalizedString("The file %@ is identified as as a generic MAD tracker, and not a specific one. Renaming it will fix this. Do you want to rename the file extension?", comment: "Invalid extension description"), defaultButton: NSLocalizedString("Rename", comment: "rename file"), alternateButton: NSLocalizedString("Open", comment:"Open a file"), otherButton: NSLocalizedString("Cancel", comment: "Cancel"), args: theURL.lastPathComponent);
			switch (retVal) {
			case NSAlertDefaultReturn:
				
				var rec: NSDictionary? = nil
				var ostype = [Int8](count: 5, repeatedValue: 0)
				
				var identified = madLib.identifyFileAtURL(theURL, type: &ostype)
				
				if (madLib.identifyFileAtURL(theURL, type: &ostype) != .NoErr) || madLib.getInformationFromFileAtURL(theURL, type: &ostype, infoDictionary: &rec) != .NoErr {
					PPRunCriticalAlertPanel(NSLocalizedString("Unknown File", comment: "unknown file"), message: NSLocalizedString("The file type could not be identified.", comment: "Unidentified file"));
					return false;
				}
				var sigVala: AnyObject = rec![kPPSignature] ?? NSNumber(unsignedInt: "madk")
				var sigValb: MADFourChar = (sigVala as NSNumber).unsignedIntValue
				var sigVal = sigValb.stringValue
				
				
				var tmpURL = theURL.URLByDeletingPathExtension.URLByAppendingPathExtension(sigVal.lowercaseString);
				var err: NSError? = nil
				if (NSFileManager.defaultManager().moveItemAtURL(theURL, toURL:tmpURL, error:&err) == false) {
					println("Could not move file, error: \(err!)");
					PPRunInformationalAlertPanel(NSLocalizedString("Rename Error", comment: "Rename Error"), message: NSLocalizedString("The file could not be renamed to \"%@\".\n\nThe music file \"%@\" will still be loaded.", comment: "Could not rename file"), args: tmpURL.lastPathComponent, theURL.lastPathComponent);
				} else {
					theURL = tmpURL;
					//TODO: regenerate the UTI
				}
				
				break;
				
			case NSAlertAlternateReturn:
				break;
				
				//case NSAlertOtherReturn:
			default:
				return false;
			}
		}
		if (sharedWorkspace.type(theUTI, conformsToType:PPMusicListUTI)) {
			if (musicListWillChange()) {
				changeValueForMusicListKey({ () -> Void in
					self.musicList.loadMusicListAtURL(theURL)
					self.selMusFromList = self.musicList.selectedMusic
				})
				musicListDidChange()
				return true
			}
		} else if (sharedWorkspace.type(theUTI, conformsToType:PPOldMusicListUTI)) {
			if (musicListWillChange()) {
				self.willChangeValueForKey(kMusicListKVO)
				musicList.beginLoadingOfMusicListAtURL(theURL, completionHandle: { (theErr) -> Void in
					self.selMusFromList = self.musicList.selectedMusic
					self.didChangeValueForKey(kMusicListKVO)
					if theErr != nil {
						NSAlert(error: theErr).runModal()
					}
				})
				return true;
			}
		} else {
			//TODO: check for valid extension.
			for aUTI in trackerUTIs {
				if (sharedWorkspace.type(theUTI, conformsToType:aUTI)) {
					addMusicToMusicList(theURL)
					return true;
				}
			}
		}
		return false;
	}
	
	func musicListWillChange() -> Bool {
		return false
	}
	
	@IBAction func saveMusicList(sender: AnyObject!) {
		var savePanel = NSSavePanel()
		savePanel.allowedFileTypes = [PPMusicListUTI]
		savePanel.canCreateDirectories = true
		savePanel.canSelectHiddenExtension = true
		savePanel.beginSheetModalForWindow(self.window, completionHandler: { (result) -> Void in
			if result == NSFileHandlingPanelOKButton {
				self.musicList.saveMusicListToURL(savePanel.URL)
			}
		})
	}
	
	@IBAction func fastForwardButtonPressed(sender: AnyObject!) {
	
	}
	
	@IBAction func loopButtonPressed(sender: AnyObject!) {
	
	}
	
	@IBAction func nextButtonPressed(sender: AnyObject!) {
		var err: NSError? = nil
		
		if (self.currentlyPlayingIndex.index + 1 < musicList.countOfMusicList) {
			currentlyPlayingIndex.index++;
			selectCurrentlyPlayingMusic()
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error:err).runModal()
			}
		} else if (NSUserDefaults.standardUserDefaults().boolForKey(PPLoopMusicWhenDone)) {
			currentlyPlayingIndex.index = 0;
			selectCurrentlyPlayingMusic()
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error:err).runModal()
			}
		} else {
			NSBeep();
		}
	}
	
	@IBAction func playButtonPressed(sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.play()
			self.paused = false;
		}
	}
	
	@IBAction func prevButtonPressed(sender: AnyObject!) {
		if (currentlyPlayingIndex.index > 0) {
			currentlyPlayingIndex.index--;
			selectCurrentlyPlayingMusic()
			var err: NSError? = nil
			if (!loadMusicFromCurrentlyPlayingIndexWithError(&err)) {
				NSAlert(error:err).runModal()
			}
		} else {
			NSBeep();
		}
	}
	
	func beginExportSettingsWithHandler((Int) -> Void) {
		
	}
	
	func saveMusicAsAIFFToURL(aURL: NSURL, usingSettings: UnsafeMutablePointer<MADDriverSettings>) -> MADErr {
		return MADErr.UnknownErr
	}
	
	@IBAction func rewindButtonPressed(sender: AnyObject!) {
	var tag = (sender as NSMenuItem).tag;
	madDriver.beginExport()
	var savePanel = NSSavePanel()
	
	switch (tag) {
		case -1: //AIFF
		
			savePanel.allowedFileTypes = [AVFileTypeAIFF]
			savePanel.canCreateDirectories = true
			savePanel.canSelectHiddenExtension = true
			if musicName.isEmpty {
			savePanel.nameFieldStringValue = musicName
			}
			
			savePanel.prompt = "Export"
			savePanel.title = "Export as AIFF audio"
			savePanel.beginSheetModalForWindow(self.window, completionHandler: { (result) -> Void in
				
			})
			savePanel.beginSheetModalForWindow(self.window, completionHandler: { (result) -> Void in
				if (result != NSFileHandlingPanelOKButton) {
					self.madDriver.endExport()
					return
				}
				
				self.beginExportSettingsWithHandler( { (result) -> Void in
					if (result != NSAlertDefaultReturn) {
					self.madDriver.endExport()
						return;
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), {
						autoreleasepool {
							var thErr = self.saveMusicAsAIFFToURL(savePanel.URL, usingSettings: &self.exportSettings);
					self.madDriver.endExport()
							if (thErr != .NoErr) {
								return;
							}
						}
						dispatch_async(dispatch_get_main_queue(), {
							if (self.isQuitting) {
							NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
							} else {
								var retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"%@\" is complete.", defaultButton: "OK", alternateButton: "Show File", args: savePanel.URL.lastPathComponent);
								if (retVal == NSAlertAlternateReturn) {
									NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([savePanel.URL])
								}
							}
						});
					})
				})
			})
			break;
			
		case -2: //MP4
			savePanel.allowedFileTypes = ["com.apple.m4a-audio"];
			savePanel.canCreateDirectories = true
			savePanel.canSelectHiddenExtension = true
			if (musicName != "") {
				savePanel.nameFieldStringValue = musicName
			}
			savePanel.prompt = "Export"
			savePanel.title = "Export as MPEG-4 Audio"
			savePanel.beginSheetModalForWindow(self.window, completionHandler: {(result) -> Void in
				if (result != NSFileHandlingPanelOKButton) {
					self.madDriver.endExport()
					return;
				}
				
				self.beginExportSettingsWithHandler( { (result) -> Void in
					if result != NSAlertDefaultReturn {
					self.madDriver.endExport()
						return;
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), {
						var theErr = MADErr.NoErr;
						
						var oldURL = self.musicList.objectInMusicListAtIndex(self.previouslyPlayingIndex.index).musicURL
						var expErr: NSError? = nil;
						var errBlock: () -> Void = {
							if (self.isQuitting) {
								NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
							} else {
								PPRunAlertPanel("Export failed", message: "Export/coversion of the music file failed:\n%@", args: expErr!.localizedDescription);
							}
						};
						let oldMusicName = self.musicName;
						let oldMusicInfo = self.musicInfo;
						let tmpName = oldMusicName != "" ? oldMusicName : "untitled"
						var tmpURL = NSFileManager.defaultManager().URLForDirectory(.ItemReplacementDirectory, inDomain: .UserDomainMask, appropriateForURL: oldURL, create: true, error: nil).URLByAppendingPathComponent("\(tmpName).aiff", isDirectory: false)
						
						theErr = self.saveMusicAsAIFFToURL(tmpURL, usingSettings:&self.exportSettings)
						if theErr != .NoErr {
							expErr = CreateErrorFromMADErrorType(theErr);
							NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
							dispatch_async(dispatch_get_main_queue(), errBlock);
						}
						
						
						var exportMov = AVAsset.assetWithURL(tmpURL) as AVAsset!
							var metadataInfo = generateAVMetadataInfo(oldMusicName, oldMusicInfo)
						
						if (exportMov == nil) {
							expErr = NSError(domain: NSCocoaErrorDomain, code: NSFileWriteUnknownError, userInfo: nil)
							NSLog("Init Failed for %@, error: %@", oldMusicName, expErr!.localizedDescription);
							NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
							dispatch_async(dispatch_get_main_queue(), errBlock);
							return;
						}
						
						var session = AVAssetExportSession(asset:exportMov, presetName:AVAssetExportPresetAppleM4A)
						if (session == nil) {
							expErr = NSError(domain: NSCocoaErrorDomain, code: NSFileWriteUnknownError, userInfo: nil)
							NSLog("Export session creation for %@ failed, error: %@", oldMusicName, expErr!.localizedDescription);
							NSFileManager.defaultManager().removeItemAtURL(tmpURL, error: nil)
							dispatch_async(dispatch_get_main_queue(), errBlock);
							return;
						}
						NSFileManager.defaultManager().removeItemAtURL(savePanel.URL, error: nil)
						session.outputURL = savePanel.URL
						session.outputFileType = AVFileTypeAppleM4A;
						session.metadata = metadataInfo;
						var sessionWaitSemaphore = dispatch_semaphore_create(0);
						session.exportAsynchronouslyWithCompletionHandler({ () -> Void in
							var tmp = dispatch_semaphore_signal(sessionWaitSemaphore)
							})
						dispatch_semaphore_wait(sessionWaitSemaphore, DISPATCH_TIME_FOREVER);
						
						var didFinish = session.status == .Completed;
						NSFileManager.defaultManager().removeItemAtURL(savePanel.URL, error: nil)
						
						if (didFinish) {
							dispatch_async(dispatch_get_main_queue(), {
								if (self.isQuitting) {
									NSApplication.sharedApplication().replyToApplicationShouldTerminate(true)
								} else {
									var retVal = PPRunInformationalAlertPanel("Export complete", message: "The export of the file \"%@\" is complete.", defaultButton: "OK", alternateButton: "Show File", args: savePanel.URL.lastPathComponent);
									if (retVal == NSAlertAlternateReturn) {
										NSWorkspace.sharedWorkspace().activateFileViewerSelectingURLs([savePanel.URL])
									}
								}
							});
						} else {
							NSLog("\(session.error)");
						}
					});
				});
			});
			
		case -3: // wave
			/*
			[savePanel setAllowedFileTypes:@[AVFileTypeWAVE]];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![self.musicName isEqualToString:@""])
				[savePanel setNameFieldStringValue:self.musicName];
			
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:@"Export as Wave Audio"];
			[savePanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result) {
				if (result != NSFileHandlingPanelOKButton) {
					[madDriver endExport];
					return;
				}
				[self beginExportSettingsWithHandler:^(NSInteger result) {
					if (result != NSAlertDefaultReturn) {
						[madDriver endExport];
						return;
					}
					
					dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
						@autoreleasepool {
							NSData *saveData = [self rawLESoundData:&exportSettings];
							[madDriver endExport];
							
							if (!saveData)
								return;
							
							__block NSError *expErr = nil;
							dispatch_block_t errBlock = ^{
								if (isQuitting) {
									[NSApp replyToApplicationShouldTerminate:YES];
								} else {
									PPRunAlertPanel(@"Export failed", @"Export of the music file failed:\n%@", nil, nil, nil, [expErr localizedDescription]);
								}
							};


				#define checkError(res) { \
if (res != MADNoErr){ \
expErr = [NSError errorWithDomain:NSOSStatusErrorDomain code:res userInfo:nil];\
dispatch_async(dispatch_get_main_queue(), errBlock);\
return; \
} \
}

		AudioStreamBasicDescription asbd = {0};
							asbd.mSampleRate = exportSettings.outPutRate;
							asbd.mFormatID = kAudioFormatLinearPCM;
							asbd.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
							asbd.mBitsPerChannel = exportSettings.outPutBits;
							switch (exportSettings.outPutMode) {
								case MonoOutPut:
									asbd.mChannelsPerFrame = 1;
									break;
									
								default:
								case StereoOutPut:
								case DeluxeStereoOutPut:
									asbd.mChannelsPerFrame = 2;
									break;
									
								case PolyPhonic:
									asbd.mChannelsPerFrame = 4;
									break;
							}
							asbd.mFramesPerPacket = 1;
							asbd.mBytesPerFrame = asbd.mBitsPerChannel * asbd.mChannelsPerFrame / 8;
							asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;
							
							CFURLRef url = CFBridgingRetain([savePanel URL]);
							
							AudioFileID audioFile;
							OSStatus res;
							res = AudioFileCreateWithURL(url, kAudioFileWAVEType, &asbd, kAudioFileFlags_EraseFile, &audioFile);
							CFRelease(url);
							checkError(res);
							
							UInt32 numBytes = (UInt32)[saveData length];
							
							res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, [saveData bytes]);
							checkError(res);
							
							res = AudioFileClose(audioFile);
							checkError(res);
//#undef checkError
							
						}
						dispatch_async(dispatch_get_main_queue(), ^{
							if (isQuitting) {
								[NSApp replyToApplicationShouldTerminate:YES];
							} else {
								NSInteger retVal = PPRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"OK", @"Show File", nil, [[savePanel URL] lastPathComponent]);
								if (retVal == NSAlertAlternateReturn) {
									[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[[savePanel URL]]];
								}
							}
						});
					});
				}];
			}];
*/
			break;
			
		default:
			/*
			if (tag > madLib.pluginCount || tag < 0) {
				NSBeep();
				[madDriver endExport];
				if (isQuitting) {
					[NSApp replyToApplicationShouldTerminate:YES];
				}
				
				return;
			}
			[savePanel setAllowedFileTypes:[madLib pluginAtIndex:tag].UTItypes];
			[savePanel setCanCreateDirectories:YES];
			[savePanel setCanSelectHiddenExtension:YES];
			if (![self.musicName isEqualToString:@""]) {
				[savePanel setNameFieldStringValue:self.musicName];
			}
			[savePanel setPrompt:@"Export"];
			[savePanel setTitle:[NSString stringWithFormat:@"Export as %@", [madLib pluginAtIndex:tag].menuName]];
			
			[savePanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result) {
				if (result != NSFileHandlingPanelOKButton) {
					[madDriver endExport];
					return;
				}
				
				NSURL *fileURL = [savePanel URL];
				OSErr err = [self.music exportMusicToURL:fileURL format:[madLib pluginAtIndex:tag].plugType library:madLib];
				if (err != MADNoErr) {
					if (isQuitting) {
						[NSApp replyToApplicationShouldTerminate:YES];
					} else {
						NSError *aerr = CreateErrorFromMADErrorType(err);
						[[NSAlert alertWithError:aerr] runModal];
					}
				} else {
					[self addMusicToMusicList:fileURL loadIfPreferencesAllow:NO];
					if (isQuitting) {
						[NSApp replyToApplicationShouldTerminate:YES];
					} else {
						NSInteger retVal = PPRunInformationalAlertPanel(@"Export complete", @"The export of the file \"%@\" is complete.", @"OK", @"Show File", nil, [[savePanel URL] lastPathComponent]);
						if (retVal == NSAlertAlternateReturn) {
							[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:@[fileURL]];
						}
					}
				}
			}];
*/
			break;

}
}
	
	@IBAction func sliderChanged(sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.setMusicStatusWithCurrentTime(Int(songPos.doubleValue), maximumTime: Int(songPos.maxValue), minimumTime:0)
		}
	}
	
	@IBAction func stopButtonPressed(sender: AnyObject!) {
		if (self.music != nil) {
			madDriver.stop()
			madDriver.cleanDriver()
			madDriver.setMusicStatusWithCurrentTime(0, maximumTime: 100, minimumTime: 0)
			self.paused = true;
		}
	}
	
	@IBAction func pauseButtonPressed(sender: AnyObject!) {
		if (self.music != nil) {
			if (self.paused) {
				madDriver.play()
			} else {
				madDriver.pause()
			}
			self.paused = !paused;
		}
	}

	@IBAction func exportMusicAs(sender: AnyObject!) {
	
	}
	
	func application(sender: NSApplication!, openFile filename: String!) -> Bool {
		var err: NSError? = nil
		var utiFile = NSWorkspace.sharedWorkspace().typeOfFile(filename, error: &err)
		if (err != nil) {
			var tmpAlert = NSAlert()
			tmpAlert.alertStyle = .WarningAlertStyle
			tmpAlert.messageText = "Error opening file"
			tmpAlert.informativeText = "Unable to open \(filename.lastPathComponent): \(err?.localizedFailureReason)"
			PPRunAlertPanel("Error opening file", message: "Unable to open %@: %@", args: filename.lastPathComponent, err!.localizedFailureReason);
			tmpAlert.runModal()
			return false;
		}
		return handleFile(NSURL(fileURLWithPath: filename), ofType: utiFile)
	}

	private func badTracker() {
		fileName.stringValue = PPDoubleDash
		internalName.stringValue = PPDoubleDash
		fileSize.stringValue = PPDoubleDash
		musicInstrument.stringValue = PPDoubleDash
		musicPatterns.stringValue = PPDoubleDash
		musicPlugType.stringValue = PPDoubleDash
		musicSignature.stringValue = PPDoubleDash
		fileLocation.stringValue = PPDoubleDash
	}
	
	func tableViewSelectionDidChange(notification: NSNotification!) {
		var selected = tableView.selectedRowIndexes
		var musicURL: NSURL! = nil
		var theInfo: NSDictionary? = nil
		//var theInfo = PPInfoRec()
		//var obj: PPMusicListObject! = nil
		//var swiftInfo = ContiguousArray<UInt>(count: 5, repeatedValue: 0)
		var info = [Int8](count: 5, repeatedValue: 0)
		var NSSig = ""
		
		if (selected.count > 0) {
			musicList.selectedMusic = selected.firstIndex
		}
		
		if (selected.count != 1) {
		 badTracker()
			return
		}
		
		var obj = musicList.objectInMusicListAtIndex(selected.lastIndex)
		
		musicURL = obj.musicURL;
		if (madLib.identifyFileAtURL(musicURL, type:&info) != MADErr.NoErr) {
		 badTracker()
			return
		}
		if (madLib.getInformationFromFileAtURL(musicURL, type:&info, infoDictionary:&theInfo) != MADErr.NoErr) {
			badTracker()
			return
		}
		fileName.stringValue = obj.fileName
		var intNameV: AnyObject! = theInfo?[kPPInternalFileName] ?? ""
		internalName.stringValue = intNameV as String
		
		let fsInt: AnyObject! = theInfo?[kPPFileSize] ?? NSNumber(long:0)
		let fsInta = fsInt as NSNumber
		let fsTmp = fsInta.longValue
		fileSize.integerValue = fsTmp
		
		let ti: AnyObject! = theInfo?[kPPTotalInstruments] ?? NSNumber(short: 0)
		let tia = ti as NSNumber
		let tiTmp = tia.shortValue
		musicInstrument.integerValue = Int(tiTmp);
		
		let mp: AnyObject! = theInfo?[kPPTotalPatterns] ?? NSNumber(short: 0)
		let mpa = mp as NSNumber
		let mpTmp = mpa.shortValue
		musicPatterns.integerValue = Int(mpTmp)
		
		let musPlugTyp: AnyObject! = theInfo?[kPPFormatDescription] ?? ""
		musicPlugType.stringValue = musPlugTyp as String
		
		let sig: AnyObject! = theInfo?[kPPSignature] ?? NSNumber(unsignedInt:0)
		let siga: MADFourChar = (sig as NSNumber).unsignedIntValue
		NSSig = siga.stringValue
		if (NSSig == nil) {
			NSSig = NSString(format: "0x%08X", siga)
		}
		musicSignature.stringValue = NSSig
		
		fileLocation.stringValue = musicURL.path
	}
	
	func tableView(tableView: NSTableView!, acceptDrop info: NSDraggingInfo!, row: Int, dropOperation: NSTableViewDropOperation) -> Bool {
		var dragPB = info.draggingPasteboard()
		var tmpArray = dragPB.readObjectsForClasses([PPMusicListDragClass.self], options: nil)
		if (tmpArray != nil && tmpArray.count != 0) {
			var minRow = 0;
			var tmpDragClass: AnyObject = tmpArray[0]
			var dragClass = tmpDragClass as PPMusicListDragClass
			var dragIndexSet = dragClass.theIndexSet;
			
			var currentIndex = dragIndexSet.firstIndex;
			while currentIndex != NSNotFound {
				if currentIndex <= row {
					minRow++;
				}
				currentIndex = dragIndexSet.indexGreaterThanIndex(currentIndex)
			}
			
			changeValueForMusicListKey({
				var selArray = self.musicList.arrayOfObjectsInMusicListAtIndexes(dragIndexSet)
				self.musicList.removeObjectsInMusicListAtIndexes(dragIndexSet)
				self.musicList.insertObjects(selArray, inMusicListAtIndex: row - minRow)
			})
			musicListContentsDidMove()
			return true;
		}
		tmpArray = dragPB.readObjectsForClasses([NSURL.self], options:[NSPasteboardURLReadingFileURLsOnlyKey : true,
			NSPasteboardURLReadingContentsConformToTypesKey : self.trackerUTIs])
		if (tmpArray != nil) {
			if (tmpArray.count < 1) {
				return false;
			}
			
			changeValueForMusicListKey({
				var mutArray = [PPMusicListObject]()
				for acurURL in tmpArray {
					let curURL = acurURL as NSURL
					mutArray.append(PPMusicListObject(URL: curURL))
				}
				self.musicList.insertObjects(mutArray, inMusicListAtIndex: row)
			})
			musicListContentsDidMove()
			return true;
		}
		
		return false;
	}
	
	func tableView(tableView1: NSTableView!, validateDrop info: NSDraggingInfo!, proposedRow row: Int, proposedDropOperation dropOperation: NSTableViewDropOperation) -> NSDragOperation {
		var result = NSDragOperation.None;
		
		if (info.draggingSource() === tableView1) {
			result = .Move;
			//TODO: check for number of indexes that are greater than the drop row.
			tableView1.setDropRow(row, dropOperation: .Above)
		} else {
			var pb = info.draggingPasteboard();
			
			//list the file type UTIs we want to accept
			var acceptedTypes = self.trackerUTIs;
			
			var urls = pb.readObjectsForClasses([NSURL.self], options: [NSPasteboardURLReadingFileURLsOnlyKey : true,
				NSPasteboardURLReadingContentsConformToTypesKey : acceptedTypes]);
			
			if (urls.count > 0) {
				result = .Copy
				tableView1.setDropRow(row, dropOperation: .Above)
			}
		}
		
		return result;
	}
	
	func tableView(tableView: NSTableView!, toolTipForCell cell: NSCell!, rect: NSRectPointer, tableColumn: NSTableColumn!, row: Int, mouseLocation: NSPoint) -> String! {
		if (row >= 0 || row <= musicList.countOfMusicList) {
			return nil;
		}
		return musicList.URLAtIndex(row).path
	}
	
	func tableView(tableView: NSTableView!, writeRowsWithIndexes rowIndexes: NSIndexSet!, toPasteboard pboard: NSPasteboard!) -> Bool {
		var status = false;
		var dragClass = PPMusicListDragClass(indexSet: rowIndexes)
		var urlArrays = [NSURL]()
		var ppmobjects = musicList.arrayOfObjectsInMusicListAtIndexes(rowIndexes)
		for obj in ppmobjects {
			urlArrays.append(obj.musicURL)
		}
		var tmpObjs:[AnyObject] = [dragClass] + urlArrays
		pboard.clearContents(); // clear pasteboard to take ownership
		status = pboard.writeObjects(tmpObjs) // write the URLs
		return status;
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
